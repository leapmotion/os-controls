#pragma once
#include <memory>

class ExposeView;

class ExposeViewAccessManager {
public:
  ExposeViewAccessManager(void);

private:
  std::weak_ptr<ExposeView> m_weakLock;

public:
  std::shared_ptr<ExposeView> Lock(void);
  bool IsLocked(void) const;
};