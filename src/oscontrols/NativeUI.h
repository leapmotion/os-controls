#pragma once

struct NativeCallbacks {
  /// <summary>
  /// Invoked by the UI when the user has indicated that we should quit
  /// </summary>
  void(*OnQuit)();
};

/// <summary>
/// Registers the system tray icon with the system to allow user interaction
/// </summary>
void ShowUI(const NativeCallbacks& callbacks);

/// <summary>
/// Causes any previously displayed notification area UI to be destroyed
/// </summary>
/// <remarks>
/// This operation is permanent and is called before application termination.  This method is idempotent.
/// </remarks>
void DestroyUI(void);