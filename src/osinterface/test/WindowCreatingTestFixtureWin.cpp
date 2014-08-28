#include "stdafx.h"
#include "WindowCreatingTestFixtureWin.h"

WindowCreatingTestFixtureWin::~WindowCreatingTestFixtureWin(void) {
  for(HANDLE createdHandle : createdHandles) {
    TerminateProcess(createdHandle, -1);
    CloseHandle(createdHandle);
  }
}

WindowCreatingTestFixture* WindowCreatingTestFixture::New(void) {
  return new WindowCreatingTestFixtureWin;
}

TestWindowProperties WindowCreatingTestFixtureWin::CreateTestWindow(void) {
  TestWindowProperties retVal;
  retVal.title = "Calculator";

  STARTUPINFOW startupinfo = {};
  startupinfo.cb = sizeof(startupinfo);

  PROCESS_INFORMATION procinfo = {};
  if(!CreateProcessW(L"C:\\Windows\\system32\\calc.exe", nullptr, nullptr, nullptr, false, 0, nullptr, nullptr, &startupinfo, &procinfo))
    throw std::runtime_error("Failed to create test process");

  retVal.pid = procinfo.dwProcessId;
  CloseHandle(procinfo.hThread);
  createdHandles.push_back(procinfo.hProcess);
  return retVal;
}