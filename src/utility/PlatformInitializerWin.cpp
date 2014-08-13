#include "stdafx.h"
#include "PlatformInitializerWin.h"
#include <Objbase.h>

PlatformInitializer::PlatformInitializer(void) :
  m_hr(CoInitializeEx(nullptr, COINIT_MULTITHREADED))
{
  if(FAILED(m_hr))
    throw std::runtime_error("Failed to initialize COM for multithreading");
}

PlatformInitializer::~PlatformInitializer(void)
{
  if(SUCCEEDED(m_hr))
    CoUninitialize();
}
