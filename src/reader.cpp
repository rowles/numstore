#include <vector>

#include "mapped.h"
#include "series.h"

/*
class ZReader {

  public:
    ZReader(int i) {
      this->i = i;
    }

    int get_i() const {
      return i;
    }

    std::vector<int> get_v() const {
      return {1,2,3,i};
    }

    void write(std::vector<uint32_t> nums) const {
      std::string file_path="test.vec";
    const series::header hdr{.size = nums.size(),
                             .dtype = series::numeric_type::Int64};

    std::unique_ptr<mmapped::mmap_file> mem_buf =
        std::make_unique<mmapped::mmap_file>(file_path,
                                             mmapped::mmap_file::Mode::CR);

      series::write(mem_buf, hdr, nums);
    }

    std::vector<uint32_t> read() const {
      std::string file_path="test.vec";
    std::unique_ptr<mmapped::mmap_file> mem_buf =
        std::make_unique<mmapped::mmap_file>(file_path,
                                             mmapped::mmap_file::Mode::RO);
    mem_buf->open();
    const auto size = series::read_header(mem_buf).size;
    const auto data = series::read(mem_buf);
    std::vector<uint32_t> xxxx;
   // xxxx.assign(*data.get(), *data.get()+size);
    xxxx.assign(data.get(), data.get()+size);
    return xxxx;
    }

  private:
    int i;
};*/
