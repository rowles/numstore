#pragma once

#include <cstring>
#include <sstream>
#include <vector>
#include <span>
#include <functional>

#include "TurboPFor-Integer-Compression/vp4.h"
#include "mapped.h"


namespace chunk_series {

using IntType = uint64_t;

enum class Algo {
  TurboPFor = 0,
  TurboPForV = 1,
  TurboPForDelta = 2,
  TurboPForDeltaZigZag = 3,
};

using CompressFunct = std::function<size_t(std::uint64_t* in, size_t n, unsigned char* out)>;
using DecompressFunct = std::function<size_t(unsigned char* in, size_t n, std::uint64_t* out)>;

inline CompressFunct get_compress_funct(const Algo& algo) noexcept {
  CompressFunct functor;

  switch(algo) {
    case Algo::TurboPFor:  functor = p4nenc64; break;
    case Algo::TurboPForV: functor = p4nenc128v64; break;
    case Algo::TurboPForDelta: functor = p4ndenc64; break;
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

/**
 * File Layout
 *
 *  ------
 *  | File Header
 *  ------
 *  | Chunk 0 Header
 *  ------
 *  | Chunk ... Header
 *  ------
 *  | Chunk n Header
 *  ------
 *  | Data 0 Chunk
 *  ------
 *  | Data ... Chunk
 *  ------
 *  | Data n Chunk
 *  ------
 *
 **/

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
  ChunkedReader() = default;
  ~ChunkedReader() = default;

  void open(std::string file_path);
  size_t size() const;

  Headers read_headers() const noexcept;

  // Read chunks into a buffer
  //
  // Return number of elements reads
  std::size_t read_into_buffer(IntType* buffer, size_t size) const noexcept;
  std::size_t read_into_buffer(std::span<IntType> buffer) const noexcept;

  // TODO: parallel reads would be pretty easy

private:
  std::shared_ptr<mmapped::mmap_file> mem_buf;
};

class ChunkedWriter {
public:
  ChunkedWriter(const Algo& _algo) : compress_funct(get_compress_funct(_algo)) {};
  ~ChunkedWriter() = default;

  void open(std::string file_path, size_t size);

  // Write array into file
  //
  // Return bytes written
  size_t write(std::span<IntType> &vec, std::uint64_t chunk_size=1000000);
  size_t write(IntType* data, size_t size);

private:
  std::shared_ptr<mmapped::mmap_file> mem_buf;
  const CompressFunct compress_funct;

};


} // namespace chunk_series

