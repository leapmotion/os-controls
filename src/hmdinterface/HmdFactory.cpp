#include "HMDFactory.h"
#include "OculusRift/RiftContext.h"
#include "OculusRift/RiftDevice.h"

namespace Hmd{
  //At the moment, we only have one kind of HMD implementation which makes this a lot easier.
  HmdFactory::HmdFactory() :
    m_context(new OculusRift::Context)
  {

  }

  HmdFactory::~HmdFactory() {
    m_context->Shutdown();
  }

  IDevice* HmdFactory::CreateDevice() {
    if (!m_context->IsInitialized())
      m_context->Initialize();

    OculusRift::Context* context = dynamic_cast<OculusRift::Context*>(m_context.get());
    auto device = new OculusRift::Device(*context);
    return device;
  }
};