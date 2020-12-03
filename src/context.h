#ifndef _CONTEXT_
#define _CONTEXT_

#include "device.h"

class Context {
  const Device     device;
  const cl_context context;
 public:
  Context(const Device& device);

  class CommandQueue;
  class MemoryBuffer;
  class Program;
  class Kernel;
  CommandQueue createCommandQueue() const;
  MemoryBuffer createMemoryBuffer(cl_mem_flags flags, long count, size_t size) const;
  Program      createProgram(std::string kernel) const;
  void         release() const; // FIXME: should be destructor after lifecycle is managed
  
  class CommandQueue {
    const cl_command_queue queue;
    CommandQueue(const Context& context);
   public:
    void writeBuffer(const MemoryBuffer& buffer, const void *source) const;
    void writeBuffer(const MemoryBuffer& buffer, size_t offset, size_t length, const void *source) const;
    void readBuffer (const MemoryBuffer& buffer, void *dest) const;
    void readBuffer (const MemoryBuffer& buffer, size_t offset, size_t length, void *dest) const;
    void flush() const;
    void finish() const;
    void release() const; // FIXME: should be destructor after lifecycle is managed
    friend Context;
  };
  friend CommandQueue;

  class MemoryBuffer {
    const cl_mem buffer;
    MemoryBuffer(const Context& context, const cl_mem_flags flags, long count, size_t size);
   public:
    const size_t length;
    void release() const; // FIXME: should be destructor after lifecycle is managed
    friend Context;
  };
  friend MemoryBuffer;

  class Program {
    Program(const Context& context, std::string kernel);
   public:
    const Device      device;
    const cl_program  program;
    const std::string kernel;
    Kernel createKernel(std::string endpoint) const;
    void release() const; // FIXME: should be destructor after lifecycle is managed
    friend Context;
  };
  friend Program;

  class Kernel {
    Kernel(const Program& program, std::string endpoint);
   public:
    const cl_kernel kernel;
    void setArg(cl_uint index, size_t size, const void *value) const;
    void executeNDRange(const CommandQueue& queue, cl_ulong count, cl_ulong batch=64) const;
    void release() const; // FIXME: should be destructor after lifecycle is managed
    friend Program;
  };
  friend Kernel;
};

#endif // _CONTEXT_
