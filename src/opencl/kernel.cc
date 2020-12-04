#include "kernel.h"

#include <CL/cl.h>
#include <fstream>

#define MAX_SOURCE_SIZE (0x100000)

Kernel::Kernel(string s)
  :source(s) { }

string Kernel::read(string filename) {
  std::ifstream input(filename);
  if (! input) {
    throw string("Unable to read kernel ")+filename;
  }
  return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}
