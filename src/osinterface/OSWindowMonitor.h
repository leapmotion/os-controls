#pragma once
#include "OSGeometry.h"
#include "uievents/Updatable.h"
#include <functional>

class OSWindow;

/// <summary>
/// Top-level monitor type used to monitor the OS for window events
/// </summary>
class OSWindowMonitor:
  public ContextMember,
  public Updatable
{
public:
  OSWindowMonitor(void);
  ~OSWindowMonitor(void);

  static OSWindowMonitor* New(void);

public:
  /// <summary>
  /// Returns a window from the specified virtual desktop coordinate
  /// </summary>
  /// <remarks>
  /// The returned window is a top-level desktop window which can receive focus.  If
  /// no window is below the specified point, this method may return nullptr.
  /// </remarks>
  virtual OSWindow* WindowFromPoint(const OSPoint& pt) const = 0;

  /// <summary>
  /// Enumeration routine, used to list all windows presently known to the monitor
  /// </summary>
  virtual void Enumerate(const std::function<void(OSWindow&)>& callback) const = 0;
};

