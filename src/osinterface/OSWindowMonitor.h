#pragma once
#include "OSGeometry.h"
#include "uievents/Updatable.h"
#include <functional>

class OSWindow;

/// <summary>
/// Top-level monitor type used to monitor the OS for window creation and destruction events
/// </summary>
/// <seealso cref="OSWindowEvent">
/// This type describes the circumstances under which important events are raised
/// </seealso>
class OSWindowMonitor:
  public ContextMember,
  public Updatable
{
public:
  OSWindowMonitor(void);
  ~OSWindowMonitor(void);

  static OSWindowMonitor* New(void);

protected:
  /// <summary>
  /// Routine to scan and update the window list
  /// </summary>
  /// </remarks>
  /// Called periodically from within the Tick() method
  /// </remarks>
  virtual void Scan(void) = 0;

public:
  /// <summary>
  /// Enumeration routine, used to list all windows presently known to the monitor
  /// </summary>
  virtual void Enumerate(const std::function<void(OSWindow&)>& callback) const = 0;

  // Updatable overrides:
  void Tick(std::chrono::duration<double> deltaT) override;
};
