#include "device.h"
#include <cassert>
#include <string>
#include <cstdio>

static const int max = 16;

vector<Device> Device::devices;

static std::string readString(cl_device_info infoType, const cl_device_id deviceID) {
  size_t size;
  assert(CL_SUCCESS == clGetDeviceInfo(deviceID , infoType, 0, NULL, &size));
  char   buffer[size];
  assert(CL_SUCCESS == clGetDeviceInfo(deviceID, infoType, size, buffer, NULL));
  return std::string(buffer);
}

static cl_uint readUInt(cl_device_info infoType, const cl_device_id deviceID) {
  cl_uint result;
  assert(CL_SUCCESS == clGetDeviceInfo(deviceID, infoType, sizeof(result), &result, NULL));
  return result;
}

static cl_ulong readULong(cl_device_info infoType, const cl_device_id deviceID) {
  cl_ulong result;
  assert(CL_SUCCESS == clGetDeviceInfo(deviceID, infoType, sizeof(result), &result, NULL));
  return result;
}

static std::string readType(const cl_device_id deviceID) {
  const cl_ulong type = readULong(CL_DEVICE_TYPE, deviceID);
  std::string concat;
  if (type & CL_DEVICE_TYPE_CPU)         concat += ",CPU";
  if (type & CL_DEVICE_TYPE_GPU)         concat += ",GPU";
  if (type & CL_DEVICE_TYPE_ACCELERATOR) concat += ",ACCELERATOR";
  if (type & CL_DEVICE_TYPE_DEFAULT)     concat += ",DEFAULT";
  return concat.empty() ? "NONE" : std::string(concat.c_str()+1);
}

Device::Device(const Platform& platform, const cl_device_id did)
 :deviceID(did),
  type(readType(did)),
  name(readString(CL_DEVICE_NAME, did)),
  vendor(readString(CL_DEVICE_VENDOR, did)),
  deviceVersion(readString(CL_DEVICE_VERSION, did)),
  driverVersion(readString(CL_DRIVER_VERSION, did)),
  maxComputeUnits(readUInt(CL_DEVICE_MAX_COMPUTE_UNITS, did)),
  maxClockFrequency(readUInt(CL_DEVICE_MAX_CLOCK_FREQUENCY, did)),
  globalMemorySize(readULong(CL_DEVICE_GLOBAL_MEM_SIZE, did)) { };

Device::Device(const Device& other)
 :deviceID(other.deviceID),
  type(other.type),
  name(other.name),
  vendor(other.vendor),
  deviceVersion(other.deviceVersion),
  driverVersion(other.driverVersion),
  maxComputeUnits(other.maxComputeUnits),
  maxClockFrequency(other.maxClockFrequency),
  globalMemorySize(other.globalMemorySize) { };

Device& Device::operator=(const Device& other) {
  if (this != &other) {
    this->deviceID = other.deviceID;
  }
  return *this;
};

std::vector<Device> Device::get() {
  if (devices.size() == 0) {
    vector<Device> d;
    for (const Platform& platform : Platform::get()) {
      cl_device_id deviceIDs[max];
      cl_uint      ret_num_devices;
      switch (clGetDeviceIDs(platform.platformID, CL_DEVICE_TYPE_ALL, max, deviceIDs, &ret_num_devices)) {
        case CL_SUCCESS: {
          for (cl_uint j=0; j<ret_num_devices; ++j) {
            d.emplace_back(Device(platform, deviceIDs[j]));
          }
          break;
        }
        case CL_DEVICE_NOT_FOUND: break;
        default: throw "Unable to scan devices";
      }
    }
    devices = d;
  }
  return devices;
}

std::ostream& operator<<(std::ostream& os, const Device& device) {
  return os
      << "type=" << device.type
      << ", device=" << device.name
      << ", vendor=" << device.vendor
      << ", deviceVersion=" << device.deviceVersion
      << ", driverVersion=" << device.driverVersion
      << ", maxComputeUnits=" << device.maxComputeUnits
      << ", maxClockFrequency=" << device.maxClockFrequency
      << ", globalMemorySize=" << device.globalMemorySize;
}
