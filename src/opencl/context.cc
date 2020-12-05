#include "context.h"

static cl_context createContext(ptr<Device> device) {
  cl_int ret=CL_SUCCESS;
  const cl_context context = clCreateContext(NULL, 1, &device->deviceID, NULL, NULL, &ret);
  assert(CL_SUCCESS == ret);
  return context;
}

Context::Context(ptr<Device> device)
 :device(device), context(createContext(device)) {
  assert(device->ready());
}

Context::~Context() {
  assert(CL_SUCCESS == clReleaseContext(context));
}

ptr<Context::CommandQueue> Context::createCommandQueue() const {
  return ptr<Context::CommandQueue>(new CommandQueue(ref()));
}

ptr<Context::MemoryBuffer> Context::createMemoryBuffer(cl_mem_flags flags, long count, size_t size) const {
  MemoryBuffer* mb = new MemoryBuffer(ref(), flags, count, size);
  return mb->bake();
}

ptr<Context::Program> Context::createProgram(std::string kernel) const {
  Program* program = new Program(ref(), kernel);
  return program->bake();
}



static cl_command_queue createCQ(const cl_context& context, ptr<Device> device) {
  cl_int ret=CL_SUCCESS;
  cl_command_queue queue = clCreateCommandQueueWithProperties(context, device->deviceID, NULL, &ret);
  assert(CL_SUCCESS == ret);
  return queue;
}

Context::CommandQueue::CommandQueue(ptr<Context> context)
 :queue(createCQ(context->context, context->device)) { }

Context::CommandQueue::~CommandQueue() {
  flush();
  finish();
  assert(CL_SUCCESS == clReleaseCommandQueue(queue));
}

void Context::CommandQueue::writeBuffer(ptr<MemoryBuffer> buffer, const void *source) const {
  writeBuffer(buffer, 0, buffer->length, source);
}

void Context::CommandQueue::writeBuffer(ptr<MemoryBuffer> buffer, size_t offset, size_t length, const void *source) const {
  assert(CL_SUCCESS == clEnqueueWriteBuffer(queue, buffer->buffer, CL_TRUE, offset, length, source, 0, NULL, NULL));
}

void Context::CommandQueue::readBuffer(ptr<MemoryBuffer> buffer, void *dest) const {
  readBuffer(buffer, 0, buffer->length, dest);
}

void Context::CommandQueue::readBuffer(ptr<MemoryBuffer> buffer, size_t offset, size_t length, void *dest) const {
  assert(CL_SUCCESS == clEnqueueReadBuffer(queue, buffer->buffer, CL_TRUE, offset, length, dest, 0, NULL, NULL));
}

void Context::CommandQueue::flush() const {
  assert(CL_SUCCESS == clFlush(queue));
}

void Context::CommandQueue::finish() const {
  assert(CL_SUCCESS == clFinish(queue));
}




static cl_mem createMB(const cl_context& context, cl_mem_flags flags, size_t bufferSize, void *hostPtr) {
  cl_int ret=CL_SUCCESS;
  cl_mem buffer = clCreateBuffer(context, flags, bufferSize, hostPtr, &ret);
  assert(CL_SUCCESS == ret);
  return buffer;
}

Context::MemoryBuffer::MemoryBuffer(ptr<Context> context, const cl_mem_flags flags, long count, size_t size)
  :buffer(createMB(context->context, flags, count*size, NULL)), length(count*size) { }

Context::MemoryBuffer::~MemoryBuffer() {
  assert(CL_SUCCESS == clReleaseMemObject(buffer));
}




static cl_program createProg(const cl_context& context, std::string kernel) {
  cl_int ret=CL_SUCCESS;
  const char*  chars  = kernel.c_str();
  const size_t length = kernel.length();
  const cl_program program = clCreateProgramWithSource(context, 1, &chars, &length, &ret);
  assert(CL_SUCCESS == ret);
  return program;
}

Context::Program::Program(ptr<Context> context, std::string kernel)
 :device(context->device), program(createProg(context->context, kernel)) {
  // FIXME: no support for build options
  assert(CL_SUCCESS == clBuildProgram(program, 1, &device->deviceID, NULL, NULL, NULL));
}

Context::Program::~Program() {
  assert(CL_SUCCESS == clReleaseProgram(program));
}

ptr<Context::Kernel> Context::Program::createKernel(std::string kernel) const {
  Kernel* k = new Kernel(ref(), kernel);
  return k->bake();
}



static cl_ulong defaultBatchSize = 64;

static cl_kernel createKern(const cl_program& program, std::string endpoint) {
  cl_int ret=CL_SUCCESS;
  cl_kernel kernel = clCreateKernel(program, endpoint.c_str(), &ret);
  assert(CL_SUCCESS == ret);
  return kernel;
}

Context::Kernel::Kernel(ptr<Context::Program> program, std::string endpoint)
  :kernel(createKern(program->program, endpoint)) { }

Context::Kernel::~Kernel() {
  assert(CL_SUCCESS == clReleaseKernel(kernel));
}

void Context::Kernel::setArg(cl_uint index, size_t size, const void *value) const {
  assert(CL_SUCCESS == clSetKernelArg(kernel, index, size, value));
}

cl_ulong Context::Kernel::inferBatchSize(cl_ulong count) const {
  if (count < defaultBatchSize) {
    return count;
  } else if (count % defaultBatchSize == 0) {
    return defaultBatchSize;
  } else {
    return 1;
  }
}

void Context::Kernel::executeNDRange(ptr<Context::CommandQueue> queue, cl_ulong count) const {
  const cl_ulong batch = inferBatchSize(count);
  if (batch == 1) {
    std::cerr << "WARNING: default batch size " << defaultBatchSize << "does not divide count " << count << "; using batch size of " << batch << std::endl;
  }
  executeNDRange(queue, count, batch);
}

void Context::Kernel::executeNDRange(ptr<Context::CommandQueue> queue, cl_ulong count, cl_ulong batch) const {
  assert(CL_SUCCESS == clEnqueueNDRangeKernel(queue->queue, kernel, 1, NULL, 
					      &count, &batch, 0, NULL, NULL));
}
