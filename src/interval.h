#ifndef _INTERVAL_
#define _INTERVAL_

#include <cmath>
#include <CL/cl.h>

typedef struct __attribute__ ((packed)) _st_Interval {
  cl_double min,max;

  _st_Interval()
   :min(NAN), max(NAN) { }
  _st_Interval(cl_double min, cl_double max)
   :min(min), max(max) { }
  _st_Interval& operator=(const _st_Interval& other) {
    min = other.min;
    max = other.max;
    return *this;
  }
  bool operator==(const _st_Interval& other) {
    return min==other.min && max==other.max;
  }
  bool empty() const {
    return std::isnan((double)min) || std::isnan((double)max);
  }
  operator bool() const {
    return ! empty();
  }
  static bool nonempty(const _st_Interval& interval) {
    return ! interval.empty();
  }
} Interval;

#endif // _INTERVAL_
