#ifndef _SUBDIVIDER_
#define _SUBDIVIDER_

#include <vector>
#include "opencl/context.h"
#include "opencl/kernel.h"
#include "intervals.h"

template<int N>
class Subdivider {
  const ptr<Device>                device;
  const ptr<Context>               context;
  const ptr<Context::CommandQueue> queue;
  const ptr<Context::Program>      program;
  const ptr<Context::Kernel>       kernel;
 public:
  Subdivider(const Kernel& kernel);

  Intervals<N> subdivide(Intervals<N> ranges);
  Intervals<N> compact(Intervals<N> ranges);
};

#endif // _SUBDIVIDER_
