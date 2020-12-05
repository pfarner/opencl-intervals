#include <iterator>
#include "subdivider.h"

template<int N>
Subdivider<N>::Subdivider(const Kernel& kernel)
 :device(Device::getGPU()),
  context(device->createContext()),
  queue(context->createCommandQueue()),
  program(context->createProgram(kernel.source)),
  kernel(program->createKernel("interval_select")) {
}

template<int N>
Intervals<N> Subdivider<N>::subdivide(Intervals<N> input) {
  if (input.empty()) return input;
  
  ptr<Context::MemoryBuffer> a_mem_obj = context->createMemoryBuffer(CL_MEM_READ_ONLY,  input.size(), input.degree*sizeof(Interval));
  ptr<Context::MemoryBuffer> r_mem_obj = context->createMemoryBuffer(CL_MEM_WRITE_ONLY, input.size(), input.degree*sizeof(Interval));
  ptr<Context::MemoryBuffer> s_mem_obj = context->createMemoryBuffer(CL_MEM_WRITE_ONLY, input.size(), input.degree*sizeof(Interval));

  queue->writeBuffer(a_mem_obj, input.volumes->data());

  kernel->setArg(0, sizeof(cl_mem), &a_mem_obj->buffer);
  kernel->setArg(1, sizeof(cl_mem), &r_mem_obj->buffer);
  kernel->setArg(2, sizeof(cl_mem), &s_mem_obj->buffer);

  kernel->executeNDRange(queue, input.size());

  Intervals<N> result(input.degree, 2*input.size());
  queue->readBuffer(r_mem_obj, result.volumes->data());
  queue->readBuffer(s_mem_obj, result.volumes->data()+input.size());

  return result;
}

template<int N>
static void pad(ptr<Context::Kernel> kernel, Intervals<N>& intervals) {
  if (! intervals.empty()) {
    unsigned int size = intervals.size();
    unsigned int next = std::bit_ceil(size);
    if (next > size) {
      if (kernel->inferBatchSize(next) < next) {
	std::cerr << "padding " << intervals.size() << " to " << next << std::endl;
      }
      intervals.volumes->resize(next);
    }
  }
}

template<int N>
Intervals<N> Subdivider<N>::compact(Intervals<N>& input) {
  Intervals<N> result(input.degree);
  std::back_insert_iterator bii(*(result.volumes));
  
  // FIXME: this is done in the CPU, and could be on GPU using a prefix scan to choose placement
  // see https://documen.tician.de/pyopencl/algorithm.html
  // std::copy_if(input.volumes->begin(), input.volumes->end(), bii, [](std::array<Interval,N>& volume) {
  //     for (const Interval& interval : volume) {
  // 	if (interval.empty()) return false;
  //     }
  //     return true;
  //   });
  std::copy_if(input.volumes->begin(), input.volumes->end(), bii, Intervals<N>::nonemptyVolume);
  pad(kernel, result);

  // FIXME: for consistency, should an arbitrary sort be put here?  May be necessary for progress detection
  
  return result;
}
