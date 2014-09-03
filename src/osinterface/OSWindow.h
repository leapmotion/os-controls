#pragma once
#include "OSPoint.h"
#include "OSWindowNode.h"
#include <cstdint>

class ImagePrimitive;

struct OSSize {
  uint32_t cx;
  uint32_t cy;
};

/// <summary>
/// A platform-independent representation of a single window
/// </summary>
class OSWindow:
  public std::enable_shared_from_this<OSWindow>,
  public OSWindowNode
{
public:
  OSWindow(void);
  virtual ~OSWindow(void);

public:
  /// <returns>
  /// A unique identifier for this window
  /// </returns>
  virtual uint64_t GetWindowID(void) const = 0;

  /// <summary>
  /// Copies an image corresponding to a window into the specified image primitive
  /// </summary>
  virtual void GetWindowTexture(ImagePrimitive& img) = 0;

  /// <returns>
  /// True if this window has focus
  /// </returns>
  virtual bool GetFocus(void) = 0;

  /// <summary>
  /// Makes this window a foreground window
  /// </summary>
  virtual void SetFocus(void) = 0;

  /// <returns>
  /// The title of this window
  /// </returns>
  /// <remarks>
  /// The window title is not the same as the application name in that the window title is
  /// only what's represented to the user, and therefore can change over time
  /// </remarks>
  virtual std::wstring GetTitle(void) = 0;

  /// <returns>
  /// The location of the top-left corner of the window
  /// </returns>
  virtual OSPoint GetPosition(void) = 0;

  /// <returns>
  /// The real size of this window
  /// </retunrs>
  virtual OSSize GetSize(void) = 0;

  /// <summary>
  /// Prevents the specified window from being rendered on-screen
  /// </summary>
  virtual void Cloak(void) = 0;

  /// <summary>
  /// Reverses a prior call to Cloak
  /// </summary>
  virtual void Uncloak(void) = 0;

  bool operator==(const OSWindow& rhs) const {
    return GetWindowID() == rhs.GetWindowID();
  }
};

