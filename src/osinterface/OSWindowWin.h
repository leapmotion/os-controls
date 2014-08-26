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
  std::vector<std::shared_ptr<OSWindowNode>> EnumerateChildren(void) override;

  // OSWindow overrides;
  std::wstring GetTitle(void) override;
  OSPoint GetPosition(void) override;
  void Cloak(void) override;
  void Uncloak(void) override;
};

static_assert(!std::is_abstract<OSWindowWin>::value, "OSWindowWon is meant to be a concrete type");