#pragma once
#include <autowiring/ContextMember.h>
struct NativeUI:
  public ContextMember
{
  NativeUI(void);

  /// <summary>
  /// Registers the system tray icon with the system to allow user interaction
  /// </summary>
  void ShowUI(void);

  /// <summary>
  /// Causes any previously displayed notification area UI to be destroyed
  /// </summary>
  /// <remarks>
  /// This operation is permanent and is called before application termination.  This method is idempotent.
  /// </remarks>
  void DestroyUI(void);

  /// <summary>
  /// Invoked by the UI when the user has indicated that we should quit
  /// </summary>
  void OnQuit(void);

  /// <summary>
  /// Invoked when we want to display a toolbar notification message
  /// </summary>
  void ShowToolbarMessage(const char* title, const char* message);
};
