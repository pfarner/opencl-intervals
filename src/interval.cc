#include <array>
#include <iomanip>
#include "opencl/kernel.h"
#include "opencl/context.h"
#include "subdivider.h"

ptr<std::vector<Interval>> subdivide(ptr<std::vector<Interval>> input) {
  return input;
}

static std::ostream& operator<<(std::ostream& os, ptr<std::vector<Interval>> intervals) {
  for (const Interval& interval : *intervals) {
    if (interval) {
      os << '[' << (double)interval.min << ',' << (double)interval.max << ']' << " (delta=" << (double)interval.max-(double)interval.min << ')' << std::endl;
    }
  }
  return os;
}

int main(void) {
  Subdivider subdivider(Kernel::read("interval.cl"));

  const int count = 1;
  ptr<std::vector<Interval>> input(new std::vector<Interval>());
  for(int i=0; i<count; ++i) {
    input->push_back(Interval(i/(double)count,(i+1)/(double)count));
  }
  
  ptr<std::vector<Interval>> subdivided = input;

  std::cout << std::setprecision(16);
  std::cout << input << std::endl;
  for (int i=0; i<100; ++i) {
    //ptr<std::vector<Interval>> before = subdivided;
    //std::cout << "before:" << std::endl << &*before << std::endl << before << std::endl;
    subdivided = subdivider.subdivide(subdivided);
    subdivided = subdivider.compact(subdivided);
    //std::cout << "after:"  << std::endl << &*subdivided << std::endl << subdivided << std::endl;
    //if (*before == *subdivided) break; // FIXME: this exits when the vectors are not equal!
    std::cout << subdivided << std::endl;
    if (subdivided->empty()) break;
  }

  return 0;
}
