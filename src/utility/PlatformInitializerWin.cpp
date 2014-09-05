#include "stdafx.h"
#include "PlatformInitializerWin.h"
#include <Objbase.h>

PlatformInitializer::PlatformInitializer(void) :
  m_hr(CoInitializeEx(nullptr, COINIT_MULTITHREADED))
{
  if(FAILED(m_hr))
    throw std::runtime_error("Failed to initialize COM for multithreading");


  //Compute the exe path.
  const DWORD len = GetModuleFileName(nullptr, m_appPath, MAX_PATH);

  if (len == 0) {
    throw std::runtime_error("Couldn't locate our .exe");
  }

  size_t i;
  for (i = len - 1; i > 0; i--) {
    if (m_appPath[i] == '\\') {
      break;
    }
  }

  if (i <= 0){
    throw std::runtime_error("Should have been an absolute path.");
  }

  m_appPath[i + 1] = '\0';  /* chop off filename. */
}

PlatformInitializer::~PlatformInitializer(void)
{
  if(SUCCEEDED(m_hr))
    CoUninitialize();
}