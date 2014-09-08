#pragma once
#include "OSWindow.h"
#include "utility/HandleUtilitiesWin.h"
#include <type_traits>

class OSWindowWin:
  public OSWindow
{
public:
  OSWindowWin(HWND hwnd);
  ~OSWindowWin(void);

  const HWND hwnd;

private:
  // Resources required to get a texture of the underlying window
  unique_ptr_of<HBITMAP> m_hBmp;
  unique_ptr_of<HDC> m_hBmpDC;

  // Pointer to bitmap bits, may invalidate if HBITMAP is freed
  void* m_phBitmapBits;

  // Size of the bitmap the above structures
  SIZE m_szBitmap;

public:
  // PMPL routines:
  void SetZOrder(int zOrder) {
    m_zOrder = zOrder;
  }

  // OSWindow overrides:
  bool IsValid(void) override;
  uint32_t GetOwnerPid(void) override;
  std::shared_ptr<OSApp> GetOwnerApp(void) override;
  uint64_t GetWindowID(void) const override { return (uint64_t) hwnd; }
  std::shared_ptr<ImagePrimitive> GetWindowTexture(std::shared_ptr<ImagePrimitive> img) override;
  bool GetFocus(void) override;
  void SetFocus(void) override;
  std::wstring GetTitle(void) override;
  OSPoint GetPosition(void) override;
  OSSize GetSize(void) override;
  void Cloak(void) override;
  void Uncloak(void) override;
  bool IsVisible(void) const override;
};

static_assert(!std::is_abstract<OSWindowWin>::value, "OSWindowWin is meant to be a concrete type");
