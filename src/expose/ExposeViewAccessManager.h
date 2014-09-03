#pragma once
#include <memory>

class ExposeView;
class ExposeViewController;

class ExposeViewAccessManager {
public:
  ExposeViewAccessManager(void);

private:
  AutoCreateContext ctxt;

  // All of the stuff required for an expose view:
  AutoRequired<ExposeView> m_exposeView;
  AutoRequired<ExposeViewController> m_exposeViewController;

  // Pointer to the view proper, unexpired as long as someone holds the return value of Lock
  std::weak_ptr<ExposeView> m_weakLock;

public:
  std::shared_ptr<ExposeView> Lock(void);
  bool IsLocked(void) const;
};