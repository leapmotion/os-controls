/*==================================================================================================================

    Copyright (c) 2010 - 2013 Leap Motion. All rights reserved.

  The intellectual and technical concepts contained herein are proprietary and confidential to Leap Motion, and are
  protected by trade secret or copyright law. Dissemination of this information or reproduction of this material is
  strictly forbidden unless prior written permission is obtained from Leap Motion.

===================================================================================================================*/

#if !defined(__LPWindowWin_h__)
#define __LPWindowWin_h__

#include "LPWindowBase.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#if defined(min)
#undef min
#endif
#if defined(max)
#undef max
#endif

class LPWindowWin : public LPWindowBase {
  public:
    LPWindowWin(bool isTransparent, bool isDoubleBuffered);
    virtual ~LPWindowWin();

    virtual LPPoint Postion() const override;
    virtual LPSize Size() const override;
    virtual LPRect Rect() const override;

    virtual void SetPosition(const LPPoint& position) override;
    virtual void SetSize(const LPSize& size) override;
    virtual void SetRect(const LPRect& rect) override;

    virtual void Show(bool show = true) override;
    virtual void Hide() override;

    virtual void Redraw() override;

  private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    int Create(HWND hWnd);

    HWND m_hWnd;
    HGLRC m_renderingContext;
};

typedef LPWindowWin LPWindowPlatform;

#endif // __LPWindowWin_h__
