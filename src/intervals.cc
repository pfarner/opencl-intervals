#include "intervals.h"

template<int N>
Intervals<N>::Intervals(cl_int degree)
 :degree(degree), volumes(new std::vector<std::array<Interval,N>>()) { }

template<int N>
Intervals<N>::Intervals(cl_int degree, uint count)
 :degree(degree), volumes(new std::vector<std::array<Interval,N>>()) {
  volumes->resize(count);
}

template<int N>
Intervals<N>::Intervals(cl_int degree, ptr<std::vector<std::array<Interval,N>>> volumes)
 :degree(degree), volumes(volumes) { }

template<int N>
Intervals<N>& Intervals<N>::operator=(const Intervals<N>& other) {
  volumes = other.volumes;
  return *this;
}

template<int N>
std::ostream& operator<<(std::ostream& os, Intervals<N> intervals) {
  for (const std::array<Interval,N>& volume : *(intervals.volumes)) {
    if (! Intervals<N>::nonemptyVolume(volume)) continue;
    
    const char* prefix = "";
    double scale = 1;
    for (const Interval& interval : volume) {
      os << prefix << '[' << interval.min << ',' << interval.max << ']';
      prefix = " × ";
      scale *= interval.max-interval.min;
    }
    os << " (volume=" << scale << ')' << std::endl;
  }
  return os;
}

template<int N>
bool Intervals<N>::empty() const {
  return volumes->empty();
}

template<int N>
bool Intervals<N>::nonempty() const {
  return ! empty();
}

template<int N>
uint Intervals<N>::size() const {
  return volumes->size();
}

template<int N>
bool Intervals<N>::nonemptyVolume(const std::array<Interval,N>& volume) {
  for (const Interval& interval : volume) {
    if (interval.empty()) return false;
  }
  return true;
}
