#pragma once
#include "OSWindowNode.h"
#include <cstdint>

struct OSPoint {
  uint32_t x;
  uint32_t y;
};

/// <summary>
/// A platform-independent representation of a single window
/// </summary>
class OSWindow:
  public OSWindowNode
{
public:
  OSWindow(void);
  virtual ~OSWindow(void);

public:
  /// <returns>True if this window is still valid</returns>
  /// <remarks>
  /// A window handle can become invalid for many reasons.  The most likely cause, generally,
  /// is that the window itself has closed while enumeration is underway.  Representations of
  /// invalid handles should generally be destroyed as quickly as possible, as no operations
  /// on an invalid handle can be valid.
  /// </remarks>
  virtual bool IsValid(void) = 0;

  /// <returns>
  /// The title of this window
  /// </returns>
  /// <remarks>
  /// The window title is not the same as the application name in that the window title is
  /// only what's represented to the user, and therefore can change over time
  /// </remarks>
  virtual std::wstring GetTitle(void) = 0;

  /// <returns>
  /// The PID of the owning application
  /// </returns>
  virtual uint32_t GetOwningPid(void) = 0;

  /// <returns>
  /// The location of the top-left corner of the window
  /// </returns>
  virtual OSPoint GetPosition(void) = 0;

  /// <summary>
  /// Prevents the specified window from being rendered on-screen
  /// </summary>
  virtual void Cloak(void) = 0;

  /// <summary>
  /// Reverses a prior call to Cloak
  /// </summary>
  virtual void Uncloak(void) = 0;
};

