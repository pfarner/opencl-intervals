#ifndef _platform_h_
#define _platform_h_

#include <CL/cl.h>
#include <iostream>
#include <vector>
#include "ptr.h"

using std::vector;
using std::iostream;

class Platform : public Pointable<Platform> {
  static vector<ptr<Platform>> platforms;
 public:
  Platform(const cl_platform_id platformID);

  cl_platform_id platformID;
  std::string    name;

  static vector<ptr<Platform>> get();

  friend std::ostream& operator<<(std::ostream& os, ptr<Platform> platform);
};

#endif // _platform_h_
