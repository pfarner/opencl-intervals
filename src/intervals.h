#ifndef _INTERVALS_
#define _INTERVALS_

#include <array>
#include <ostream>
#include <vector>
#include "interval.h"
#include "opencl/ptr.h"

template<int N>
class Intervals {
 public:
  const uint degree;
  // This is a collection of multidimensional rectangular prisms
  ptr<std::vector<std::array<Interval, N>>> volumes;

  Intervals(uint degree);
  Intervals(uint degree, uint count);
  Intervals(uint degree, ptr<std::vector<std::array<Interval,N>>> volumes);
  Intervals<N>& operator=(const Intervals& other);

  bool empty() const;
  bool nonempty() const;
  uint size() const;
};

template<int N>
std::ostream& operator<<(std::ostream& os, Intervals<N> intervals);

#endif // _INTERVALS_
