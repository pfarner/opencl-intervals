#include <cassert>
#include "context.h"

static cl_context createContext(const Device& device) {
  cl_int ret=CL_SUCCESS;
  const cl_context context = clCreateContext(NULL, 1, &device.deviceID, NULL, NULL, &ret);
  assert(CL_SUCCESS == ret);
  return context;
}

Context::Context(const Device& device)
 :device(device), context(createContext(device)) { }

Context::CommandQueue Context::createCommandQueue() const {
  return CommandQueue(*this);
}

Context::MemoryBuffer Context::createMemoryBuffer(cl_mem_flags flags, long count, size_t size) const {
  return MemoryBuffer(*this, flags, count, size);
}

Context::Program Context::createProgram(std::string kernel) const {
  return Program(*this, kernel);
}

void Context::release() const {
  assert(CL_SUCCESS == clReleaseContext(context));
}



static cl_command_queue createCQ(const cl_context& context, const Device& device) {
  cl_int ret=CL_SUCCESS;
  cl_command_queue queue = clCreateCommandQueueWithProperties(context, device.deviceID, NULL, &ret);
  assert(CL_SUCCESS == ret);
  return queue;
}

Context::CommandQueue::CommandQueue(const Context& context)
 :queue(createCQ(context.context, context.device)) { }

void Context::CommandQueue::writeBuffer(const MemoryBuffer& buffer, const void *source) const {
  writeBuffer(buffer, 0, buffer.length, source);
}

void Context::CommandQueue::writeBuffer(const MemoryBuffer& buffer, size_t offset, size_t length, const void *source) const {
  assert(CL_SUCCESS == clEnqueueWriteBuffer(queue, buffer.buffer, CL_TRUE, offset, length, source, 0, NULL, NULL));
}

void Context::CommandQueue::readBuffer(const MemoryBuffer& buffer, void *dest) const {
  readBuffer(buffer, 0, buffer.length, dest);
}

void Context::CommandQueue::readBuffer(const MemoryBuffer& buffer, size_t offset, size_t length, void *dest) const {
  assert(CL_SUCCESS == clEnqueueReadBuffer(queue, buffer.buffer, CL_TRUE, offset, length, dest, 0, NULL, NULL));
}

void Context::CommandQueue::flush() const {
  assert(CL_SUCCESS == clFlush(queue));
}

void Context::CommandQueue::finish() const {
  assert(CL_SUCCESS == clFinish(queue));
}

void Context::CommandQueue::release() const {
  assert(CL_SUCCESS == clReleaseCommandQueue(queue));
}




static cl_mem createMB(const cl_context& context, cl_mem_flags flags, size_t bufferSize, void *hostPtr) {
  cl_int ret=CL_SUCCESS;
  cl_mem buffer = clCreateBuffer(context, flags, bufferSize, hostPtr, &ret);
  assert(CL_SUCCESS == ret);
  return buffer;
}

Context::MemoryBuffer::MemoryBuffer(const Context& context, const cl_mem_flags flags, long count, size_t size)
  :buffer(createMB(context.context, flags, count*size, NULL)), length(count*size) { }

void Context::MemoryBuffer::release() const {
  clReleaseMemObject(buffer);
}




static cl_program createProg(const cl_context& context, std::string kernel) {
  cl_int ret=CL_SUCCESS;
  const char*  chars  = kernel.c_str();
  const size_t length = kernel.length();
  const cl_program program = clCreateProgramWithSource(context, 1, &chars, &length, &ret);
  assert(CL_SUCCESS == ret);
  return program;
}

Context::Program::Program(const Context& context, std::string kernel)
 :device(context.device), program(createProg(context.context, kernel)) {
  // FIXME: no support for build options
  assert(CL_SUCCESS == clBuildProgram(program, 1, &device.deviceID, NULL, NULL, NULL));
}

Context::Kernel Context::Program::createKernel(std::string kernel) const {
  return Kernel(*this, kernel);
}

void Context::Program::release() const {
  assert(CL_SUCCESS == clReleaseProgram(program));
}



static cl_kernel createKern(const cl_program& program, std::string endpoint) {
  cl_int ret=CL_SUCCESS;
  cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);
  assert(CL_SUCCESS == ret);
  return kernel;
}

Context::Kernel::Kernel(const Context::Program& program, std::string endpoint)
  :kernel(createKern(program.program, endpoint)) { }

void Context::Kernel::setArg(cl_uint index, size_t size, const void *value) const {
  assert(CL_SUCCESS == clSetKernelArg(kernel, index, size, value));
}

void Context::Kernel::executeNDRange(const Context::CommandQueue& queue, cl_ulong count, cl_ulong batch) const {
  assert(CL_SUCCESS == clEnqueueNDRangeKernel(queue.queue, kernel, 1, NULL, 
					      &count, &batch, 0, NULL, NULL));
}

void Context::Kernel::release() const {
  assert(CL_SUCCESS == clReleaseKernel(kernel));
}
