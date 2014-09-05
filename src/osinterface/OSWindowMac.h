#pragma once
#include "OSWindow.h"
#include <type_traits>

#include <Foundation/NSDictionary.h>

class OSWindowMac:
  public OSWindow
{
public:
  OSWindowMac(NSDictionary* info);
  ~OSWindowMac(void);

  // OSWindow overrides:
  bool IsValid(void) override;
  uint32_t GetOwnerPid(void) override;
  std::shared_ptr<OSApp> GetOwnerApp(void) override;
  uint64_t GetWindowID(void) const override { return (uint64_t) m_windowID; }
  std::shared_ptr<ImagePrimitive> GetWindowTexture(const std::shared_ptr<ImagePrimitive>& img) override;
  bool GetFocus(void) override;
  void SetFocus(void) override;
  std::wstring GetTitle(void) override;
  OSPoint GetPosition(void) override;
  OSSize GetSize(void) override;
  void Cloak(void) override;
  void Uncloak(void) override;
  bool IsVisible(void) const override;

private:
  const CGWindowID m_windowID;
  NSDictionary* m_info;
  uint32_t m_mark;

  void UpdateInfo(NSDictionary* info);
  void SetMark(uint32_t mark) { m_mark = mark; }
  uint32_t Mark() const { return m_mark; }

  // PMPL routines:
  void SetZOrder(int zOrder) {
    m_zOrder = zOrder;
  }

  friend class OSWindowMonitorMac;
};

static_assert(!std::is_abstract<OSWindowMac>::value, "OSWindowMac is meant to be a concrete type");
