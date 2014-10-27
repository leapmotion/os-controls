#pragma once
#include "IContext.h"
#include "IDevice.h"

namespace Hmd {
  class HmdFactory {
  public:
    HmdFactory();
    virtual ~HmdFactory();

    IDevice* CreateDevice();
  private:
    std::unique_ptr<IContext> m_context;
  };
};
