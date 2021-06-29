#pragma once

#include <cstring>
#include <sstream>
#include <vector>
#include <span>
#include <functional>

#include "../ext/TurboPFor-Integer-Compression/vp4.h"
#include "mapped.h"


namespace chunk_series {

using IntType = uint64_t;


enum class Algo {
  TurboPFor = 0,
  TurboPForV = 1,
  TurboPForDelta = 2,
  TurboPForDelta1 = 3,
  TurboPForDeltaZigZag = 4,
};

using CompressFunct = std::function<size_t(std::uint64_t* in, size_t n, unsigned char* out)>;
using DecompressFunct = std::function<size_t(unsigned char* in, size_t n, std::uint64_t* out)>;

inline CompressFunct get_compress_funct(const Algo& algo) noexcept {
  CompressFunct functor;

  switch(algo) {
    case Algo::TurboPFor:  functor = p4nenc64; break;
    case Algo::TurboPForV: functor = p4nenc128v64; break;
    case Algo::TurboPForDelta: functor = p4ndenc64; break;
    case Algo::TurboPForDelta1: functor = p4nd1enc64; break;
    case Algo::TurboPForDeltaZigZag: functor = p4nzenc64; break;
    default: functor = p4nenc64;
  }

  return functor;
}

inline DecompressFunct get_decompress_funct(const Algo& algo) noexcept {
  DecompressFunct functor;

  switch(algo) {
    case Algo::TurboPFor:  functor = p4ndec64; break;
    case Algo::TurboPForV: functor = p4ndec128v64; break;
    case Algo::TurboPForDelta: functor = p4nddec64; break;
    case Algo::TurboPForDeltaZigZag: functor = p4nzdec64; break;
    default: functor = p4ndec64;
  }

  return functor;
}

struct FileHeader {
  std::uint64_t total_num_elements{0};
  std::uint64_t num_chunks{0};
  Algo algo{Algo::TurboPFor};

  std::string to_string() const {
    std::stringstream ss;

    ss << "total_num_elements: " << total_num_elements << '\n';
    ss << "num_chunks: " << num_chunks << '\n';

    return ss.str();
  }
};

struct ChunkHeader {
  std::uint64_t offset_pos{0};
  std::uint32_t total_bytes{0};
  std::uint32_t num_elements{0};

  std::string to_string() const {
    std::stringstream ss;

    ss << "offset_pos: " << offset_pos << '\n';
    ss << "total_bytes: " << total_bytes << '\n';
    ss << "num_elements: " << num_elements << '\n';

    return ss.str();
  }
};

struct Headers {
  const FileHeader file_header{};
  const std::span<ChunkHeader> chunk_headers{};
};

class ChunkedReader {
public:
  ChunkedReader() {}
  ~ChunkedReader() = default;

  inline void open(std::string file_path) {
    mem_buf = std::make_shared<mmapped::mmap_file>(file_path,
                                         mmapped::mmap_file::Mode::RO);
    mem_buf->open();
  }

  inline size_t size() const {
    return read_headers().file_header.total_num_elements;
  }

  inline Headers read_headers() const noexcept {
    const FileHeader fhdr = *(reinterpret_cast<FileHeader *>(mem_buf->address().get()));

    //std::cout << "--------\n";
    //std::cout << fhdr.to_string() << '\n';
    
    auto addr = reinterpret_cast<ChunkHeader *>(mem_buf->address().get() + sizeof(FileHeader));
    std::span<ChunkHeader> chunk_headers{addr, fhdr.num_chunks};

    return Headers{
      .file_header = fhdr,
      .chunk_headers = chunk_headers
    };
  }

  // Read chunks into a buffer
  //
  // Return number of elements reads
  inline std::size_t read_into_buffer(IntType* buffer, size_t size) const noexcept {
    const auto headers = read_headers();

    if (headers.file_header.total_num_elements > size) {
      return 0;
    }

    const auto decompress_funct = get_decompress_funct(headers.file_header.algo);

    const auto chunk_size = headers.file_header.total_num_elements / headers.file_header.num_chunks;
    std::unique_ptr<IntType[]> decompress_buf = std::make_unique<IntType[]>(chunk_size + 4096);
    //printf("decompress_buf size: %li\n", chunk_size + 4096*10);

    IntType* addr = buffer;

    for (auto ch : headers.chunk_headers) {
      //std::cout << ch.to_string() << '\n';
      decompress_funct(mem_buf->address().get() + ch.offset_pos, ch.num_elements, decompress_buf.get());
      //std::cout << "decompressed\n";

      std::memcpy(addr, decompress_buf.get(), sizeof(IntType) * ch.num_elements);
      addr += ch.num_elements;
    }

    return headers.file_header.total_num_elements;
  }

  // Read chunks into a span
  //
  // Return number of elements reads
  inline std::size_t read_into_buffer(std::span<IntType> buffer) const noexcept {
    return read_into_buffer(buffer.data(), buffer.size());
  }

private:
  std::shared_ptr<mmapped::mmap_file> mem_buf;
};



class ChunkedWriter {
public:
  ChunkedWriter(const Algo& _algo) : compress_funct(get_compress_funct(_algo)) {};
  ~ChunkedWriter() = default;

  inline void open(std::string file_path, size_t size) {
    mem_buf = std::make_shared<mmapped::mmap_file>(file_path,
                                       mmapped::mmap_file::Mode::CR);

    mem_buf->set_size(size);
    mem_buf->open();
  }

  inline size_t write(std::span<IntType> &vec, std::uint64_t chunk_size=1000000) {
    const std::uint64_t num_chunks = (vec.size()/chunk_size) + std::min((std::uint64_t)vec.size()%chunk_size, 1UL);

    constexpr size_t file_header_offset{sizeof(FileHeader)};
    const size_t chunk_header_offset{sizeof(ChunkHeader)*num_chunks};

    auto base_addr = mem_buf->address().get() + file_header_offset + chunk_header_offset;
    size_t chunk_offset{0};

    // iterate chunks to write
    for (std::uint64_t i = 0; i < num_chunks; ++i) {
      const auto data = vec.data() + i*chunk_size;

      // adjust chunk sizes for odd lots
      const auto size = (i == num_chunks-1 && vec.size()%chunk_size > 0) ? vec.size()%chunk_size : chunk_size;

      // write chunk data
      const size_t bytes_compressed = compress_funct(data, size, base_addr + chunk_offset);
      
      // write chunk header
      const auto offset_pos = chunk_offset + file_header_offset + chunk_header_offset;
      const ChunkHeader chdr {
        .offset_pos=offset_pos,
        .total_bytes=bytes_compressed,
        .num_elements=size
      };

      unsigned char *chdrData = (unsigned char *)(&chdr);
      auto addr = mem_buf->address().get() + file_header_offset + sizeof(ChunkHeader)*i;
      std::memcpy(addr, chdrData, sizeof(ChunkHeader));

      //printf("writing chunk, items:%li, bytes:%li, offset:%li\n", size, bytes_compressed, chunk_offset);
      //printf("%li %li \n", i*chunk_size*sizeof(IntType), data[0]);
      chunk_offset += bytes_compressed;
    }

    
    ///////
    // write_header
    const FileHeader fh{
      .total_num_elements=vec.size(),
      .num_chunks=num_chunks
    };
    unsigned char *hdrData = (unsigned char *)(&fh);
    std::memcpy(mem_buf->address().get(), hdrData, sizeof(FileHeader));

    const size_t file_size = chunk_offset + file_header_offset + chunk_header_offset;
    mem_buf->truncate_close(file_size);

    return file_size;
  }

  inline size_t write(IntType* data, size_t size) {
    std::span<IntType> tmp_span{data, size};
      return write(tmp_span);
    }

private:
  std::shared_ptr<mmapped::mmap_file> mem_buf;
  const CompressFunct compress_funct;

};


} // namespace series

