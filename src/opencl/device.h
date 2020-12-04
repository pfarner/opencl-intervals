#ifndef _device_h_
#define _device_h_

#include "platform.h"

class Context;
class Device : public Pointable<Device> {
  static vector<ptr<Device>> devices;
 public:
  Device(ptr<Platform> platform, const cl_device_id deviceID);

  cl_device_id deviceID;
  std::string  type, name, vendor, deviceVersion, driverVersion;
  cl_uint      maxComputeUnits, maxClockFrequency;
  cl_ulong     globalMemorySize;

  bool isGPU() const;

  ptr<Context> createContext() const;

  static vector<ptr<Device>> get();
  static ptr<Device>         getGPU();
  static bool                speedOrdering(ptr<Device> a, ptr<Device> b);

  friend std::ostream& operator<<(std::ostream& os, ptr<Device> platform);
};

#endif // _device_h_
