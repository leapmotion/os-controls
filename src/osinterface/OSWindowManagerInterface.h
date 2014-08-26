#pragma once
#include "OSWindowNode.h"
#include <memory>

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

  /// <summary>
  /// Enumerates all top-level windows known to the system
  /// </summary>
  virtual std::vector<std::shared_ptr<OSWindowNode>> EnumerateTopLevel(void) = 0;
};

