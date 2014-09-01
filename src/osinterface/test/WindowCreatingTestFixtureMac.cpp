#include "stdafx.h"
#include "WindowCreatingTestFixtureMac.h"

#include <ApplicationServices/ApplicationServices.h>
#include <signal.h>

WindowCreatingTestFixtureMac::~WindowCreatingTestFixtureMac(void) {
  for (auto pid : m_createdProcesses) {
    kill(pid, SIGTERM);
  }
}

WindowCreatingTestFixture* WindowCreatingTestFixture::New(void) {
  return new WindowCreatingTestFixtureMac;
}

TestWindowProperties WindowCreatingTestFixtureMac::CreateTestWindow(void) {
  TestWindowProperties retVal{"", 0};
  std::string path = "/Applications/TextEdit.app";
  FSRef fsRef;
  if (FSPathMakeRef(reinterpret_cast<const UInt8*>(path.c_str()), &fsRef, nullptr) == noErr) {
    LSApplicationParameters params = {0, kLSLaunchDefaults, &fsRef, nullptr, nullptr, nullptr};
    ProcessSerialNumber psn = { 0, 0 };
    pid_t pid = 0;
    if (LSOpenApplication(&params, &psn) == noErr &&
        GetProcessPID(&psn, &pid) == noErr) {
      retVal.title = path;
      retVal.pid = static_cast<uint32_t>(pid);
      m_createdProcesses.push_back(pid);
    }
  }
  return retVal;
}
