#include <iterator>
#include "subdivider.h"

Subdivider::Subdivider(const Kernel& kernel)
 :device(Device::getGPU()),
  context(device->createContext()),
  queue(context->createCommandQueue()),
  program(context->createProgram(kernel.source)) {
}

ptr<std::vector<Interval>> Subdivider::subdivide(ptr<std::vector<Interval>> input) {
  ptr<Context::MemoryBuffer> a_mem_obj = context->createMemoryBuffer(CL_MEM_READ_ONLY,  input->size(), sizeof(Interval));
  ptr<Context::MemoryBuffer> r_mem_obj = context->createMemoryBuffer(CL_MEM_WRITE_ONLY, input->size(), sizeof(Interval));
  ptr<Context::MemoryBuffer> s_mem_obj = context->createMemoryBuffer(CL_MEM_WRITE_ONLY, input->size(), sizeof(Interval));
  ptr<Context::Kernel>       kernel    = program->createKernel("interval_select");

  queue->writeBuffer(a_mem_obj, input->data());

  kernel->setArg(0, sizeof(cl_mem), &a_mem_obj->buffer);
  kernel->setArg(1, sizeof(cl_mem), &r_mem_obj->buffer);
  kernel->setArg(2, sizeof(cl_mem), &s_mem_obj->buffer);

  kernel->executeNDRange(queue, input->size());

  ptr<std::vector<Interval>> result(new std::vector<Interval>(2*input->size()));
  queue->readBuffer(r_mem_obj, result->data());
  queue->readBuffer(s_mem_obj, result->data()+input->size());

  return result;
}

static void pad(ptr<std::vector<Interval>> intervals) {
  if (! intervals->empty()) {
    unsigned int size = intervals->size();
    unsigned int next = std::bit_ceil(size);
    if (next > size) {
      std::cerr << "padding " << intervals->size() << " to " << next << std::endl;
      intervals->resize(next);
    }
  }
}


ptr<std::vector<Interval>> Subdivider::compact(ptr<std::vector<Interval>> input) {
  ptr<std::vector<Interval>> result(new std::vector<Interval>());
  std::back_insert_iterator bii(*result);
  
  // FIXME: this is done in the CPU, and could be on GPU using a prefix scan to choose placement
  // see https://documen.tician.de/pyopencl/algorithm.html
  std::copy_if(input->begin(), input->end(), bii, Interval::nonempty);
  pad(result);

  // FIXME: for consistency, should an arbitrary sort be put here?  May be necessary for progress detection
  
  return result;
}
