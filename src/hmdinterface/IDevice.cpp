#include "IDevice.h"
#include "autowiring/Autowired.h"
#include "HmdFactory.h"

//We know this 
namespace Hmd{
IDevice* IDevice::New() {
  AutoRequired<HmdFactory> factory;
  return factory->CreateDevice();
}
}