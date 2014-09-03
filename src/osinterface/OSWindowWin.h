#pragma once
#include "OSWindow.h"
#include <type_traits>

class OSWindowWin:
  public OSWindow
{
public:
  OSWindowWin(HWND hwnd);
  ~OSWindowWin(void);

  const HWND hwnd;

  // OSWindowNode overrides:
  bool IsValid(void) override;
  uint32_t GetOwnerPid(void) override;
  std::shared_ptr<OSApp> GetOwnerApp(void) override;
  std::vector<std::shared_ptr<OSWindowNode>> EnumerateChildren(void) override;

  // OSWindow overrides;
  uint64_t GetWindowID(void) const override { return (uint64_t) hwnd; }
  void GetWindowTexture(ImagePrimitive& texture) override;
  bool GetFocus(void) override;
  void SetFocus(void) override;
  std::wstring GetTitle(void) override;
  OSPoint GetPosition(void) override;
  OSSize GetSize(void) override;
  void Cloak(void) override;
  void Uncloak(void) override;
};

static_assert(!std::is_abstract<OSWindowWin>::value, "OSWindowWin is meant to be a concrete type");