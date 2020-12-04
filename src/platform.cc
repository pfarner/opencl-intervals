#include "platform.h"

static const int max = 16;

vector<ptr<Platform>> Platform::platforms;

static std::string readName(const cl_platform_id pid) {
  size_t size;
  clGetPlatformInfo(pid, CL_PLATFORM_PROFILE, 0, NULL, &size);
  char* profile = new char[size];
  clGetPlatformInfo(pid, CL_PLATFORM_PROFILE, size, profile, &size);
  const std::string name(profile);
  delete[] profile;
  return name;
}

Platform::Platform(const cl_platform_id pid)
 :platformID(pid), name(readName(pid)) { };

std::vector<ptr<Platform>> Platform::get() {
  if (platforms.size() == 0) {
    cl_platform_id platformIDs[max];
    cl_uint ret_num_platforms;
    assert(CL_SUCCESS == clGetPlatformIDs(max, platformIDs, &ret_num_platforms));
    vector<ptr<Platform>> p;
    for (cl_uint i=0; i<ret_num_platforms; ++i) {
      Platform* platform = new Platform(platformIDs[i]);
      p.emplace_back(platform->bake());
    }
    platforms = p;
  }
  return platforms;
}

std::ostream& operator<<(std::ostream& os, const Platform& platform) {
  return os << platform.name;
}
