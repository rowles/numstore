#include "chunk_series.h"

namespace chunk_series {

//////
// ChunkedReader
//

void ChunkedReader::open(std::string file_path) {
  mem_buf = std::make_shared<mmapped::mmap_file>(file_path,
                                        mmapped::mmap_file::Mode::RO);
  mem_buf->open();
}

size_t ChunkedReader::size() const {
  return read_headers().file_header.total_num_elements;
}

Headers ChunkedReader::read_headers() const noexcept {
  const FileHeader fhdr = *(reinterpret_cast<FileHeader *>(mem_buf->address().get()));

  auto addr = reinterpret_cast<ChunkHeader *>(mem_buf->address().get() + sizeof(FileHeader));
  std::span<ChunkHeader> chunk_headers{addr, fhdr.num_chunks};

  return Headers{
    .file_header = fhdr,
    .chunk_headers = chunk_headers
  };
}

std::size_t ChunkedReader::read_into_buffer(IntType* buffer, size_t size) const noexcept {
  const auto headers = read_headers();

  if (headers.file_header.total_num_elements > size) {
    return 0;
  }

  const auto decompress_funct = get_decompress_funct(headers.file_header.algo);

  const auto chunk_size = headers.file_header.total_num_elements / headers.file_header.num_chunks;
  std::unique_ptr<IntType[]> decompress_buf = std::make_unique<IntType[]>(chunk_size + 4194304);

  IntType* addr = buffer;

  // decompress each chunk
  for (auto ch : headers.chunk_headers) {
    decompress_funct(mem_buf->address().get() + ch.offset_pos, ch.num_elements, decompress_buf.get());

    std::memcpy(addr, decompress_buf.get(), sizeof(IntType) * ch.num_elements);
    addr += ch.num_elements;
  }

  return headers.file_header.total_num_elements;
}

std::size_t ChunkedReader::read_into_buffer(std::span<IntType> buffer) const noexcept {
  return read_into_buffer(buffer.data(), buffer.size());
}

//////
// ChunkedWriter
//
void ChunkedWriter::open(std::string file_path, size_t size) {
  mem_buf = std::make_shared<mmapped::mmap_file>(file_path,
                                     mmapped::mmap_file::Mode::CR);

  mem_buf->set_size(size);
  mem_buf->open();
}

size_t ChunkedWriter::write(std::span<IntType> &vec, std::uint64_t chunk_size) {
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
    chunk_offset += bytes_compressed;
  }

  // write file header
  const FileHeader fh{
    .total_num_elements=vec.size(),
    .num_chunks=num_chunks
  };
  unsigned char *hdrData = (unsigned char *)(&fh);
  std::memcpy(mem_buf->address().get(), hdrData, sizeof(FileHeader));

  // close mmap and truncate file to remove empty bytes
  const size_t file_size = chunk_offset + file_header_offset + chunk_header_offset;
  mem_buf->truncate_close(file_size);

  return file_size;
}

size_t ChunkedWriter::write(IntType* data, size_t size, std::uint64_t chunk_size) {
  std::span<IntType> tmp_span{data, size};
  return write(tmp_span, chunk_size);
}


} // namespace chunk_series
