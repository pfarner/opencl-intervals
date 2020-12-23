#include <fstream>
#include <sstream>
#include <iomanip>
#include "opencl/kernel.h"
#include "opencl/context.h"
#include "intervals/subdivider.h"

#include "intervals/intervals.cc"
#include "intervals/subdivider.cc"

const bool   demo = true;

template<int N>
void writeDemo(std::ostream& os, int iteration, int iterations, const Intervals<N>& intervals) {
  double totalVolume = 0;
  int    count       = 0;
  std::stringstream sb;
  sb << std::setprecision(16);
  for (const Prism<N>& prism : *(intervals.prisms)) {
    if (! Intervals<N>::nonemptyPrism(prism)) continue;
    
    double volume = 1;
    for (const Interval& interval : prism) {
      volume *= interval.max-interval.min;
    }
    totalVolume += volume;
    ++count;

    const Interval &x = prism[0], &y = prism[1], z = prism[2];
    const double pad = 1E-6;
    sb
      << x.min-pad << ' ' << y.min-pad << ' ' << z.min-pad << std::endl
      << x.min-pad << ' ' << y.min-pad << ' ' << z.max+pad << std::endl
      << x.min-pad << ' ' << y.max+pad << ' ' << z.max+pad << std::endl
      << x.min-pad << ' ' << y.max+pad << ' ' << z.min-pad << std::endl
      << x.min-pad << ' ' << y.min-pad << ' ' << z.min-pad << std::endl
      << std::endl
      << x.max+pad << ' ' << y.min-pad << ' ' << z.min-pad << std::endl
      << x.max+pad << ' ' << y.min-pad << ' ' << z.max+pad << std::endl
      << x.max+pad << ' ' << y.max+pad << ' ' << z.max+pad << std::endl
      << x.max+pad << ' ' << y.max+pad << ' ' << z.min-pad << std::endl
      << x.max+pad << ' ' << y.min-pad << ' ' << z.min-pad << std::endl
      << std::endl
      << x.min-pad << ' ' << y.min-pad << ' ' << z.min-pad << std::endl
      << x.max+pad << ' ' << y.min-pad << ' ' << z.min-pad << std::endl
      << x.max+pad << ' ' << y.max+pad << ' ' << z.min-pad << std::endl
      << x.min-pad << ' ' << y.max+pad << ' ' << z.min-pad << std::endl
      << x.min-pad << ' ' << y.min-pad << ' ' << z.min-pad << std::endl
      << std::endl
      << x.min-pad << ' ' << y.min-pad << ' ' << z.max+pad << std::endl
      << x.max+pad << ' ' << y.min-pad << ' ' << z.max+pad << std::endl
      << x.max+pad << ' ' << y.max+pad << ' ' << z.max+pad << std::endl
      << x.min-pad << ' ' << y.max+pad << ' ' << z.max+pad << std::endl
      << x.min-pad << ' ' << y.min-pad << ' ' << z.max+pad << std::endl
      << std::endl
      << std::endl;
  }
  os << "unset label" << std::endl;
  os << "set title \"subdividing ranges to find 1D solutions in a 3D space: z=xy² intersected with x+y+z==3\\niteration " << iteration
     << ": " << count << " prisms"
     << ", covering total area of " << totalVolume
     << ", next subdivision is in " << (intervals.phase==0 ? "X" : "Y") << " direction"
     << "\"" << std::endl;
  os << "splot '-' u 1:2:3 w pm3d lw 0.1 t ''"    << std::endl;
  os << sb.str() << std::endl << "EOF" << std::endl;
}

int main(void) {
  try {
    Subdivider<3> subdivider(Kernel::read("1-in-3.cl"));

    const int    count = 1;
    const double max   = 1.5;
    ptr<std::vector<Prism<3>>> input(new std::vector<Prism<3>>());
    for(int i=0; i<count; ++i) {
      for(int j=0; j<count; ++j) {
	for(int k=0; k<count; ++k) {
	  input->push_back(Prism<3>({
		Interval(i*max/count,(i+1)*max/count),
		Interval(j*max/count,(j+1)*max/count),
		Interval(k*max/count,(k+1)*max/count)
	      }));
	}
      }
    }
  
    Intervals<3> subdivided(3, input);

    std::cout << std::setprecision(16);
    std::cout << subdivided << std::endl;
    std::ofstream os("plot-1-in-3");
    os << "set samples 1000" << std::endl;
    os << "set terminal gif animate opt delay 40 optimize enhanced font \"arial,10\" fontscale 1.0 size 1200, 1200" << std::endl;
    os << "set output '1-in-3.gif~'" << std::endl;
    os << "set xrange [-0.1:1.6]" << std::endl;
    os << "set yrange [-0.1:1.6]" << std::endl;
    os << "set zrange [-0.1:1.6]" << std::endl;
    os << "set grid x y z"        << std::endl;
    os << "set style line 1 lc rgb '#b90046' lt 1 lw 0.5" << std::endl;
    os << "set pm3d depthorder hidden3d" << std::endl;
    os << "unset hidden3d" << std::endl;
    os << "unset colorbox" << std::endl;

    if (demo) writeDemo(os, 0, 0, subdivided);
    for (int i=0, n=20; i<n; ++i) {
      //ptr<std::vector<Interval>> before = subdivided;
      //std::cout << "before:" << std::endl << &*before << std::endl << before << std::endl;
      subdivided = subdivider.subdivide(subdivided);
      subdivided = subdivider.compact(subdivided);
      //std::cout << "after:"  << std::endl << &*subdivided << std::endl << subdivided << std::endl;
      //if (*before == *subdivided) break; // FIXME: this exits when the vectors are not equal!
      //std::cout << subdivided << std::endl;
      if (demo) writeDemo(os, i+1, n, subdivided);
      std::cout << "after " << i+1 << " iterations there are " << subdivided.count() << " prisms" << std::endl;
      if (subdivided.empty()) break;
    }
    os << "system \"rsync -ac 1-in-3.gif~ 1-in-3.gif\"" << std::endl;
  } catch(string str) {
    std::cerr << str << std::endl;
  } catch(const char* str) {
    std::cerr << str << std::endl;
  }

  return 0;
}
