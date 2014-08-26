#pragma once
#include <memory>
#include <vector>

class OSWindowNode;

/// <summary>
/// Represents a single window node, which is a window that has children but does not necessarily interact with the user
/// </summary>
class OSWindowNode {
public:
  virtual std::vector<std::shared_ptr<OSWindowNode>> EnumerateChildren(void) = 0;
};
