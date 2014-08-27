#pragma once
#include <functional>

class OSWindow;

/// <summary>
/// Top-level monitor type used to monitor the OS for window events
/// </summary>
class OSWindowMonitor
{
public:
  OSWindowMonitor(void);
  ~OSWindowMonitor(void);

private:
  OSWindowMonitor* New(void);

public:
  /// <summary>
  /// Enumeration routine, used to list all windows presently known to the monitor
  /// </summary>
  virtual void Enumerate(std::function<void(OSWindow&)>& callback) const = 0;
};

