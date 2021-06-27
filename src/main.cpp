#include <bits/stdint-uintn.h>
#include <cstring>
#include <iostream>
#include <iterator>
#include <span>
#include <vector>

#include "mapped.h"
#include "series.h"

// Read from stdin
template <class T>
inline void vector_from_stream(std::vector<T> &out, std::istream &in) {
  std::string line;

  for (; std::getline(in, line);) {
    const T a = std::stoi(line);
    out.push_back(a);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    //std::cerr << "Usage:" << std::endl;
    return 1;
  }

  std::string command = std::string(argv[1]);

  if (command != "write" && command != "read" && command != "info") {
    std::cerr << "command must be one of write, read, info" << std::endl;
  }

  std::string file_path;

  for (int i = 2; i < argc; ++i) {
    const auto tmpArg = std::string(argv[i]);
    if (tmpArg == "-f" || tmpArg == "--file") {
      if (i + 1 < argc) {
        file_path = std::string(argv[i + 1]);
      } else {
        std::cerr << "--file option requires one argument" << std::endl;
      }
    }
  }

  if (command == "write") {
    std::vector<series::IntType> nums{};

    vector_from_stream(nums, std::cin);
    printf("nums: %li\n", nums.size());
    const series::header hdr{.size = nums.size(),
                             .dtype = series::numeric_type::Int64};

    std::unique_ptr<mmapped::mmap_file> mem_buf =
        std::make_unique<mmapped::mmap_file>(file_path,
                                             mmapped::mmap_file::Mode::CR);

    series::write(mem_buf, hdr, nums);
  } else if (command == "read") {
    std::unique_ptr<mmapped::mmap_file> mem_buf =
        std::make_unique<mmapped::mmap_file>(file_path,
                                             mmapped::mmap_file::Mode::RO);
    mem_buf->open();
    const auto size = series::read_header(mem_buf).size;
    const auto data = series::read(mem_buf);
    std::span<series::IntType> vec{data.get(), size};

    // print to stdout
    std::ios_base::sync_with_stdio(false);
    std::copy(vec.begin(), vec.end(),
              std::ostream_iterator<series::IntType>(std::cout, "\n"));
  } else if (command == "info") {
    std::unique_ptr<mmapped::mmap_file> mem_buf =
        std::make_unique<mmapped::mmap_file>(file_path,
                                             mmapped::mmap_file::Mode::RO);
    mem_buf->open();
    std::cout << series::read_header(mem_buf).to_string() << std::endl;
  } else {
    return 1;
  }

  return 0;
}
