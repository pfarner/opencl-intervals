#ifndef _kernel_h_
#define _kernel_h_

#include <string>

using std::string;

class Kernel {
  const string source;
  
 public:
  Kernel(string source);
  
  virtual ~Kernel() { }

  static string read(string filename);
};

#endif // _kernel_h_
