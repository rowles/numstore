#pragma once

#include <cstring>
#include <sstream>
#include <vector>

#include "../ext/TurboPFor-Integer-Compression/vp4.h"
#include "mapped.h"

namespace series {

enum numeric_type { Int64, Int32, Int16, Int8 };

struct header {
  std::size_t size;
  numeric_type dtype;

  std::string to_string() const;
};

inline size_t buf_size(const size_t vec_size);

header read_header(std::unique_ptr<mmapped::mmap_file> &mem_buf);

std::unique_ptr<uint32_t[]> read(std::unique_ptr<mmapped::mmap_file> &mem_buf);

void write(std::unique_ptr<mmapped::mmap_file> &mem_buf, const header &hdr,
           std::vector<uint32_t> &vec);

} // namespace series
