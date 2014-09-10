#include "stdafx.h"
#include "OSApp.h"

OSApp::OSApp(uint32_t pid) : m_id(GetAppIdentifier(pid))
{
}

OSApp::~OSApp(void)
{
}

std::shared_ptr<OSApp> OSApp::GetAppInstance(uint32_t pid) {
  static std::mutex s_mutex;
  static std::unordered_map<std::wstring, std::weak_ptr<OSApp>> s_map;
  std::lock_guard<std::mutex> lock(s_mutex);
  const std::wstring id = OSApp::GetAppIdentifier(pid);
  if (id.empty()) {
    return std::shared_ptr<OSApp>();
  }
  auto found = s_map.find(id);
  if (found != s_map.end()) {
    std::shared_ptr<OSApp> app = found->second.lock();
    if (app) {
      return app;
    }
  }
  std::shared_ptr<OSApp> app(OSApp::New(pid));
  if (app) {
    s_map[id] = app;
  }
  return app;
}
