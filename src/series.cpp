#include "series.h"

namespace series {

std::string header::to_string() const {
  std::stringstream ss;

  ss << "Numeric Type: " << dtype << '\n';
  ss << "Size: " << size << '\n';

  return ss.str();
}

size_t buf_size(const size_t vec_size) { return vec_size * 25 / 3 + 1024; }

header read_header(std::unique_ptr<mmapped::mmap_file> &mem_buf) {
  const header hdr = *(reinterpret_cast<header *>(mem_buf->address().get()));
  return hdr;
}

std::unique_ptr<uint64_t[]> read(std::unique_ptr<mmapped::mmap_file> &mem_buf) {
  const header hdr = read_header(mem_buf);

  // get memory buffer of data past the header
  const auto baseAddr = mem_buf->address().get() + sizeof(header);
  unsigned char *arr = reinterpret_cast<unsigned char *>(baseAddr);

  // decompress from memmap and copy into buffer
  std::unique_ptr<uint64_t[]> out_buf(new uint64_t[hdr.size + 1024]);

  /*const size_t bytesRead =*/ p4ndec128v64(arr, hdr.size, out_buf.get());

  return out_buf;
}

void write(std::unique_ptr<mmapped::mmap_file> &mem_buf, const header &hdr,
           std::vector<uint64_t> &vec) {
  // compress vector
  // TODO: write to mmap directly, then maybe truncate unused bytes
  const auto bsize = buf_size(vec.size());
  std::unique_ptr<unsigned char[]> tmp_buf(new unsigned char[bsize]);
  size_t bytes_compressed = p4nenc128v64(vec.data(), vec.size(), tmp_buf.get());

  // create memmap
  const size_t buf_size = bytes_compressed + sizeof(header);
  mem_buf->set_size(buf_size);
  mem_buf->open();

  // write header and data
  unsigned char *hdrData = (unsigned char *)(&hdr);
  std::memcpy(mem_buf->address().get(), hdrData, sizeof(header));
  std::memcpy(mem_buf->address().get() + sizeof(header), tmp_buf.get(),
              bytes_compressed);

  mem_buf->sync();
}
} // namespace series
