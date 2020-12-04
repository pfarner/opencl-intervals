#include "context.h"

static const int max = 16;

static const std::string GPU_TYPE("GPU");

vector<ptr<Device>> Device::devices;

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

Device::Device(ptr<Platform> platform, const cl_device_id did)
  :deviceID(did),
   type(readType(did)),
   name(readString(CL_DEVICE_NAME, did)),
   vendor(readString(CL_DEVICE_VENDOR, did)),
   deviceVersion(readString(CL_DEVICE_VERSION, did)),
   driverVersion(readString(CL_DRIVER_VERSION, did)),
   maxComputeUnits(readUInt(CL_DEVICE_MAX_COMPUTE_UNITS, did)),
   maxClockFrequency(readUInt(CL_DEVICE_MAX_CLOCK_FREQUENCY, did)),
   globalMemorySize(readULong(CL_DEVICE_GLOBAL_MEM_SIZE, did)) { };

bool Device::speedOrdering(ptr<Device> a, ptr<Device> b) {
  const cl_ulong as = a->maxComputeUnits*(cl_ulong)a->maxClockFrequency;
  const cl_ulong bs = b->maxComputeUnits*(cl_ulong)b->maxClockFrequency;
  return as < bs;
}

std::vector<ptr<Device>> Device::get() {
  if (devices.size() == 0) {
    vector<ptr<Device>> d;
    for (ptr<Platform> platform : Platform::get()) {
      cl_device_id deviceIDs[max];
      cl_uint      ret_num_devices;
      switch (clGetDeviceIDs(platform->platformID, CL_DEVICE_TYPE_ALL, max, deviceIDs, &ret_num_devices)) {
      case CL_SUCCESS: {
	for (cl_uint j=0; j<ret_num_devices; ++j) {
	  Device* dev = new Device(platform, deviceIDs[j]);
	  d.emplace_back(dev->bake());
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

static void print(auto const& seq) {
  for (auto const& elem : seq) {
    std::cout << elem << ' ';
  }   
  std::cout << '\n';
}

bool Device::isGPU() const {
  return type==GPU_TYPE;
}

ptr<Context> Device::createContext() const {
  Context* context = new Context(ref());
  return context->bake();
}

ptr<Device> Device::getGPU() {
  const std::vector<ptr<Device>> allDevices(get());
  auto gpus(allDevices | std::views::filter([](ptr<Device> d) { return d->isGPU(); }));
  auto fastest(std::minmax_element(gpus.begin(), gpus.end(), Device::speedOrdering).second);
  if (fastest == gpus.end()) throw "No GPU found";
  return *fastest;
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
