#include <fstream>
#include <sstream>
#include <iomanip>
#include "opencl/kernel.h"
#include "opencl/context.h"
#include "intervals/subdivider.h"

#include "intervals/intervals.cc"
#include "intervals/subdivider.cc"

const bool demo = false;

template<int N>
void writeDemo(std::ostream& os, int iteration, const Intervals<N>& intervals) {
  double totalVolume = 0;
  int    count       = 0;
  for (const Prism<N>& prism : *(intervals.prisms)) {
    if (! Intervals<N>::nonemptyPrism(prism)) continue;
    
    double volume = 1;
    for (const Interval& interval : prism) {
      volume *= interval.max-interval.min;
    }
    totalVolume += volume;
    ++count;
    
    os << "set object " << count << " rect from " << prism[0].min
       << ", " << prism[1].min << " to " << prism[0].max << ", " << prism[1].max << std::endl;
    os << "set object " << count << " back clip lw 1.0 dashtype solid fc rgb \"cyan\" fillstyle solid border lt -1" << std::endl;
  }
  os << "set x2label \"iteration " << iteration
     << ": " << count << " prisms"
     << ", covering total area of " << totalVolume
     << ", next subdivision is in " << (intervals.phase==0 ? "X" : "Y") << " direction"
     << "\"" << std::endl;
  os << "plot sqrt(1-x*x) lw 3 t ''"    << std::endl;
}

int main(void) {
  try {
    Subdivider<2> subdivider(Kernel::read("interval.cl"));

    const int    count = 1;
    const double max   = 1.5;
    ptr<std::vector<Prism<2>>> input(new std::vector<Prism<2>>());
    for(int i=0; i<count; ++i) {
      for(int j=0; j<count; ++j) {
	input->push_back(Prism<2>({
	      Interval(i*max/count,(i+1)*max/count),
	      Interval(j*max/count,(j+1)*max/count)
	    }));
      }
    }
  
    Intervals<2> subdivided(2, input);

    std::cout << std::setprecision(16);
    std::cout << subdivided << std::endl;
    std::ofstream os("plot");
    os << "set samples 1000" << std::endl;
    os << "set terminal gif animate opt delay 200 optimize enhanced font \"arial,10\" fontscale 1.0 size 1200, 1200" << std::endl;
    os << "set output 'circle.gif~'" << std::endl;
    os << "set xrange [0:1.5]"            << std::endl;
    os << "set yrange [0:1.5]"            << std::endl;

    if (demo) writeDemo(os, 0, subdivided);
    for (int i=0; i<100; ++i) {
      //ptr<std::vector<Interval>> before = subdivided;
      //std::cout << "before:" << std::endl << &*before << std::endl << before << std::endl;
      subdivided = subdivider.subdivide(subdivided);
      subdivided = subdivider.compact(subdivided);
      //std::cout << "after:"  << std::endl << &*subdivided << std::endl << subdivided << std::endl;
      //if (*before == *subdivided) break; // FIXME: this exits when the vectors are not equal!
      //std::cout << subdivided << std::endl;
      if (demo) writeDemo(os, i+1, subdivided);
      std::cout << "after " << i+1 << " iterations there are " << subdivided.count() << " prisms" << std::endl;
      if (subdivided.empty()) break;
    }
    os << "system \"mv circle.gif~ circle.gif\"" << std::endl;
  } catch(string str) {
    std::cerr << str << std::endl;
  } catch(const char* str) {
    std::cerr << str << std::endl;
  }

  return 0;
}
