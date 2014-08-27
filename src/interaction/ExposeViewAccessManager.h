#pragma once
#include <memory>

class ExposeView;

class ExposeViewAccessManager {
public:
  ExposeViewAccessManager(void);

private:
  AutoCreateContext ctxt;
  AutoRequired<ExposeView> m_exposeView;
  std::weak_ptr<ExposeView> m_weakLock;

public:
  std::shared_ptr<ExposeView> Lock(void);
  bool IsLocked(void) const;
};