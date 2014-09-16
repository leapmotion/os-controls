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

  /// <summary>
  /// Causes the specified window to act as a transparent overlay
  /// </summary>
  /// <remarks>
  /// Transparent windows by default do not have a title bar, close button, or appear in the taskbar
  /// </remarks>
  void MakeTransparent(const Handle& window);
  void MakeAlwaysOnTop(const Handle& window);
  void AllowInput(const Handle& window, bool allowInput);
  void AbandonFocus(const Handle& window);
  void RaiseWindowAtPosition(float x, float y);
}
