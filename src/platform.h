#ifndef _platform_h_
#define _platform_h_

#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <string>

using std::vector;
using std::iostream;

class Platform {
  static vector<Platform> platforms;
 public:
  Platform(const cl_platform_id platformID);
  Platform(const Platform& platform);
  Platform& operator=(const Platform& other);

  cl_platform_id platformID;
  std::string    name;

  static vector<Platform> get();

  friend std::ostream& operator<<(std::ostream& os, const Platform& platform);
};

#endif // _platform_h_
