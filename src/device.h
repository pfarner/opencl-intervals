#ifndef _device_h_
#define _device_h_

#include "platform.h"

class Device {
  static vector<Device> devices;
 public:
  Device(const Platform& platform, const cl_device_id deviceID);
  Device(const Device& device);
  Device& operator=(const Device& other);

  cl_device_id deviceID;
  std::string  type, name, vendor, deviceVersion, driverVersion;
  cl_uint      maxComputeUnits, maxClockFrequency;
  cl_ulong     globalMemorySize;

  bool isGPU() const;

  static vector<Device> get();
  static Device         getGPU();
  static bool           speedOrdering(const Device& a, const Device& b);

  friend std::ostream& operator<<(std::ostream& os, const Device& platform);
};

#endif // _device_h_
