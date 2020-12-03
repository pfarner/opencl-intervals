#ifndef _kernel_h_
#define _kernel_h_

#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <CL/cl.h>

using std::string;

class Kernel {
  const string source;
  
 public:
  Kernel(string source);
  
  virtual ~Kernel() { }

  static string read(string filename);
};

#endif // _kernel_h_
