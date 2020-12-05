#ifndef _kernel_h_
#define _kernel_h_

#include <string>

using std::string;

class Kernel {
 public:
  const string source;
  
  Kernel(string source);
  
  virtual ~Kernel() { }

  static string read(string filename);
};

#endif // _kernel_h_
