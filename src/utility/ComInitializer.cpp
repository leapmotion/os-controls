#include "stdafx.h"
#include "ComInitializer.h"

#ifdef _MSC_VER
#include <Objbase.h>

ComInitializer::ComInitializer(void) :
  m_hr(CoInitializeEx(nullptr, COINIT_MULTITHREADED))
{
  if(FAILED(m_hr))
    throw std::runtime_error("Failed to initialize COM for multithreading");
}

ComInitializer::~ComInitializer(void)
{
  if(SUCCEEDED(m_hr))
    CoUninitialize();
}
#else
ComInitializer::ComInitializer(void) {}
ComInitializer::~ComInitializer(void) {}
#endif