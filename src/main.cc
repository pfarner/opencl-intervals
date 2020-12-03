#include <stdio.h>
#include <cstdlib>
#include <time.h>
#include <CL/cl.h>
#include <iostream>
#include <cassert>
#include "kernel.h"
#include "context.h"

#define MAX_SOURCE_SIZE (0x100000)

int main(void) {
  const string kernelSource = Kernel::read("kernel.cl");

  // Create the two input vectors
  const int LIST_SIZE = 1024;
  double A[LIST_SIZE], B[LIST_SIZE];
  for(int i = 0; i < LIST_SIZE; ++i) {
    A[i] = i;
    B[i] = LIST_SIZE - i;
  }

  const Device device(Device::getGPU());

  // Create an OpenCL context
  const Context context(device);

  // Create a command queue
  const Context::CommandQueue queue(context.createCommandQueue());

  // Create memory buffers on the device for each vector
  const Context::MemoryBuffer a_mem_obj = context.createMemoryBuffer(CL_MEM_READ_ONLY,  LIST_SIZE, sizeof(double));
  const Context::MemoryBuffer b_mem_obj = context.createMemoryBuffer(CL_MEM_READ_ONLY,  LIST_SIZE, sizeof(double));
  const Context::MemoryBuffer c_mem_obj = context.createMemoryBuffer(CL_MEM_WRITE_ONLY, LIST_SIZE, sizeof(double));

  // Copy the lists A and B to their respective memory buffers
  queue.writeBuffer(a_mem_obj, A);
  queue.writeBuffer(b_mem_obj, B);

  const Context::Program program(context.createProgram(kernelSource));

  Context::Kernel kernel(program.createKernel("vector_add"));

  kernel.setArg(0, sizeof(cl_mem), (void *)&a_mem_obj);
  kernel.setArg(1, sizeof(cl_mem), (void *)&b_mem_obj);
  kernel.setArg(2, sizeof(cl_mem), (void *)&c_mem_obj);

  struct timespec ts0;
  clock_gettime(CLOCK_REALTIME, &ts0);
  const double t0 = ts0.tv_sec*1E9+ts0.tv_nsec;
  printf("Starting: %f\n", t0);
  kernel.executeNDRange(queue, LIST_SIZE);
  struct timespec ts1;
  clock_gettime(CLOCK_REALTIME, &ts1);
  const double t1 = ts1.tv_sec*1E9+ts1.tv_nsec;
  printf("Done:     %f (%f)\n", t1, t1-t0);

  double C[LIST_SIZE];
  queue.readBuffer(c_mem_obj, C);

  // Print output
  for(int i = 0; i < LIST_SIZE; ++i) {
    printf("%g + %g = %g or %0.20f\n", A[i], B[i], C[i], C[i]);
  }

  queue.flush();
  queue.finish();
  kernel.release();
  program.release();
  a_mem_obj.release();
  b_mem_obj.release();
  c_mem_obj.release();
  queue.release();
  context.release();
  return 0;
}

