#include "intervals.h"

template<int N>
Intervals<N>::Intervals(cl_int degree)
 :phase(0), degree(degree), prisms(new std::vector<Prism<N>>()) { }

template<int N>
Intervals<N>::Intervals(cl_int degree, uint count)
 :phase(0), degree(degree), prisms(new std::vector<Prism<N>>()) {
  prisms->resize(count);
}

template<int N>
Intervals<N>::Intervals(cl_int degree, ptr<std::vector<Prism<N>>> prisms)
 :phase(0), degree(degree), prisms(prisms) { }

template<int N>
Intervals<N>& Intervals<N>::operator=(const Intervals<N>& other) {
  phase  = other.phase;
  prisms = other.prisms;
  return *this;
}

template<int N>
std::ostream& operator<<(std::ostream& os, Intervals<N> intervals) {
  double totalVolume = 0;
  int    count       = 0;
  for (const Prism<N>& prism : *(intervals.prisms)) {
    if (! Intervals<N>::nonemptyPrism(prism)) continue;
    
    const char* prefix = "";
    double volume = 1;
    for (const Interval& interval : prism) {
      os << prefix << '[' << interval.min << ',' << interval.max << ']';
      prefix = " × ";
      volume *= interval.max-interval.min;
    }
    os << " (volume=" << volume << ')' << std::endl;
    totalVolume += volume;
    ++count;
  }
  os << count             << " prisms";
  os << ", total volume " << totalVolume;
  os << ", degree "       << intervals.degree;
  os << ", next phase "   << intervals.phase;
  os << std::endl;
  return os;
}

template<int N>
bool Intervals<N>::empty() const {
  return prisms->empty();
}

template<int N>
bool Intervals<N>::nonempty() const {
  return ! empty();
}

template<int N>
uint Intervals<N>::capacity() const {
  return prisms->size();
}

template<int N>
uint Intervals<N>::count() const {
  int count=0;
  for (auto iter = prisms->begin(), last = prisms->end(); iter != last; ++iter) {
    if (nonemptyPrism(*iter)) ++count;
  }
  return count;
}

template<int N>
bool Intervals<N>::nonemptyPrism(const Prism<N>& prism) {
  for (const Interval& interval : prism) {
    if (interval.empty()) return false;
  }
  return true;
}
