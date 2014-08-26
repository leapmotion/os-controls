#pragma once
#include <memory>
#include <vector>

class OSWindowNode;

/// <summary>
/// Represents a single window node, which is a window that has children but does not necessarily interact with the user
/// </summary>
class OSWindowNode {
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
  /// The PID of the owning application
  /// </returns>
  virtual uint32_t GetOwnerPid(void) = 0;

  /// <summary>
  /// Enumerates all children of this window
  /// </summary>
  virtual std::vector<std::shared_ptr<OSWindowNode>> EnumerateChildren(void) = 0;
};
