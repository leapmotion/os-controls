#pragma once

namespace NativeWindow
{
  //Taken from SFML 2.1
#if _WIN32
  // Window handle is HWND (HWND__*) on Windows
  typedef HWND__* Handle;

#elif __APPLE__
  // Window handle is NSWindow (void*) on Mac OS X - Cocoa
  typedef void* Handle;

#else
  // Window handle is Window (unsigned long) on Unix - X11
  typedef unsigned long Handle;

#endif

  void MakeTransparent(const Handle& window);
  void MakeAlwaysOnTop(const Handle& window);
  void AllowInput(const Handle& window, bool allowInput);
}
