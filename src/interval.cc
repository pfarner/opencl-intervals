#include <iomanip>
#include "opencl/kernel.h"
#include "opencl/context.h"
#include "subdivider.h"

#include "intervals.cc"
#include "subdivider.cc"

int main(void) {
  Subdivider<1> subdivider(Kernel::read("interval.cl"));

  const int count = 1;
  ptr<std::vector<std::array<Interval,1>>> input(new std::vector<std::array<Interval,1>>());
  for(int i=0; i<count; ++i) {
    input->push_back(std::array<Interval,1>({ Interval(i/(double)count,(i+1)/(double)count) }));
  }
  
  Intervals<1> subdivided(1, input);

  std::cout << std::setprecision(16);
  std::cout << subdivided << std::endl;
  for (int i=0; i<100; ++i) {
    //ptr<std::vector<Interval>> before = subdivided;
    //std::cout << "before:" << std::endl << &*before << std::endl << before << std::endl;
    subdivided = subdivider.subdivide(subdivided);
    subdivided = subdivider.compact(subdivided);
    //std::cout << "after:"  << std::endl << &*subdivided << std::endl << subdivided << std::endl;
    //if (*before == *subdivided) break; // FIXME: this exits when the vectors are not equal!
    std::cout << subdivided << std::endl;
    if (subdivided.empty()) break;
  }

  return 0;
}
