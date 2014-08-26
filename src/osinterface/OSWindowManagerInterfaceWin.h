#pragma once
#include "OSWindowManagerInterface.h"
#include "OSWindowWin.h"

class OSWindowManagerInterfaceWin:
  public OSWindowManagerInterface
{
public:
  OSWindowManagerInterfaceWin(void);
  ~OSWindowManagerInterfaceWin(void);

private:
  // Root node, we keep it here to simplify our EnumerateChildren call
  OSWindowWin m_root;

public:
  std::vector<std::shared_ptr<OSWindowNode>> EnumerateTopLevel(void) override;
};

