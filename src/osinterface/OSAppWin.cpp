#include "stdafx.h"
#include "OSAppWin.h"

OSAppWin::OSAppWin(uint32_t pid) : OSApp(pid)
{
}

OSAppWin::~OSAppWin()
{
}

// Use the module file name as the application unique identifier
std::wstring OSApp::GetAppIdentifier(uint32_t pid) {
  HANDLE processHandle = OpenProcessW(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
  if (processHandle == nullptr) {
    return std::wstring();
  }
  WCHAR filename[MAX_PATH];
  std::wstring path;

  if (GetModuleFileNameEx(processHandle, nullptr, filename, MAX_PATH) != 0) {
    path = std::wstring(filename);
  }
  CloseHandle(processHandle);
  return path;
}

OSApp* OSApp::New(uint32_t pid) {
  return new OSAppWin(pid);
}

std::string OSAppWin::GetAppName(void) const {
  return std::string(); // FIXME
}

std::shared_ptr<ImagePrimitive> OSAppWin::GetIconTexture(std::shared_ptr<ImagePrimitive> img) const {
  return img; // FIXME
}
