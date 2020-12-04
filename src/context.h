#ifndef _CONTEXT_
#define _CONTEXT_

#include "device.h"

class Context : public Pointable<Context> {
  const ptr<Device>   device;
  const cl_context    context;
 public:
  Context(ptr<Device> device);
  virtual ~Context();

  class CommandQueue;
  class MemoryBuffer;
  class Program;
  class Kernel;
  ptr<CommandQueue> createCommandQueue() const;
  ptr<MemoryBuffer> createMemoryBuffer(cl_mem_flags flags, long count, size_t size) const;
  ptr<Program>      createProgram(std::string kernel) const;
  
  class CommandQueue {
    const cl_command_queue queue;
    CommandQueue(ptr<Context> context);
   public:
    virtual ~CommandQueue();
    void writeBuffer(ptr<MemoryBuffer> buffer, const void *source) const;
    void writeBuffer(ptr<MemoryBuffer> buffer, size_t offset, size_t length, const void *source) const;
    void readBuffer (ptr<MemoryBuffer> buffer, void *dest) const;
    void readBuffer (ptr<MemoryBuffer> buffer, size_t offset, size_t length, void *dest) const;
    void flush() const;
    void finish() const;
    friend Context;
  };
  friend CommandQueue;

  class MemoryBuffer : public Pointable<MemoryBuffer> {
    MemoryBuffer(ptr<Context> context, const cl_mem_flags flags, long count, size_t size);
   public:
    virtual ~MemoryBuffer();
    const cl_mem buffer;
    const size_t length;
    friend Context;
  };
  friend MemoryBuffer;

  class Program : public Pointable<Program> {
    Program(ptr<Context> context, std::string kernel);
   public:
    virtual ~Program();
    const ptr<Device> device;
    const cl_program  program;
    const std::string kernel;
    ptr<Kernel> createKernel(std::string endpoint) const;
    friend Context;
  };
  friend Program;

  class Kernel : public Pointable<Kernel> {
    Kernel(ptr<Program> program, std::string endpoint);
   public:
    virtual ~Kernel();
    const cl_kernel kernel;
    void setArg(cl_uint index, size_t size, const void *value) const;
    void executeNDRange(ptr<CommandQueue> queue, cl_ulong count, cl_ulong batch=64) const;
    friend Program;
  };
  friend Kernel;
};

#endif // _CONTEXT_
