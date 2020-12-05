#ifndef _SUBDIVIDER_
#define _SUBDIVIDER_

#include <vector>
#include "opencl/context.h"
#include "opencl/kernel.h"
#include "interval.h"

class Subdivider {
  const ptr<Device>                device;
  const ptr<Context>               context;
  const ptr<Context::CommandQueue> queue;
  const ptr<Context::Program>      program;
 public:
  Subdivider(const Kernel& kernel);

  ptr<std::vector<Interval>> subdivide(ptr<std::vector<Interval>> ranges);
  ptr<std::vector<Interval>> compact(ptr<std::vector<Interval>> ranges);
};

#endif // _SUBDIVIDER_
