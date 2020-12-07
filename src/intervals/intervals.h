#ifndef _INTERVALS_
#define _INTERVALS_

#include <array>
#include <ostream>
#include <vector>
#include "interval.h"
#include "opencl/ptr.h"

template<int N>
using Prism = std::array<Interval, N>;

template<int N>
class Intervals {
 public:
  cl_int       phase;
  const cl_int degree;
  // This is a collection of multidimensional rectangular prisms
  ptr<std::vector<Prism<N>>> prisms;

  Intervals(cl_int degree);
  Intervals(cl_int degree, uint count);
  Intervals(cl_int degree, ptr<std::vector<Prism<N>>> prisms);
  Intervals<N>& operator=(const Intervals& other);

  bool empty() const;
  bool nonempty() const;
  uint size() const;

  static bool nonemptyPrism(const Prism<N>& prism);
};

template<int N>
std::ostream& operator<<(std::ostream& os, Intervals<N> intervals);

#endif // _INTERVALS_
