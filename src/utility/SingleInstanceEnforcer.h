#pragma once

#if _WIN32
#include "HandleUtilitiesWin.h"
#include <Windows.h>

class SingleInstanceEnforcer
{
public:
  SingleInstanceEnforcer(const wchar_t* appName);
private:
  unique_ptr_of<HANDLE> m_sharedMutex;
};

#else
class SingleInstanceEnforcer {
  SingleInstanceEnforcer(const wchar_t*) {}
};
#endif