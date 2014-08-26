#pragma once
#include "OSWindowNode.h"
#include <memory>

class OSApp;
class OSWindow;

/// <summary>
/// Provides a generic abstraction of the operating system's window manager
/// </summary>
class OSWindowManagerInterface:
  public ContextMember
{
public:
  OSWindowManagerInterface(void);
  ~OSWindowManagerInterface(void);

  static OSWindowManagerInterface* New(void);

  /// <returns>
  /// The window which currently has user focus
  /// </returns>
  virtual std::shared_ptr<OSWindow> GetForegroundWindow(void) = 0;

  /// <summary>
  /// 
  /// </summary>
  virtual std::vector<std::shared_ptr<OSApp>> EnumerateInteractiveApplications(void) = 0;

  /// <summary>
  /// Enumerates all top-level windows known to the system which are also visible
  /// </summary>
  virtual std::vector<std::shared_ptr<OSWindow>> EnumerateVisibleTopLevelWindows(void) = 0;
};

