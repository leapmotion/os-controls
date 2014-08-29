#include "stdafx.h"
#include "ExposeViewAccessManager.h"
#include "ExposeView.h"

ExposeViewAccessManager::ExposeViewAccessManager(void):
  m_exposeView(ctxt)
{
}

std::shared_ptr<ExposeView> ExposeViewAccessManager::Lock(void) {
  if(!m_weakLock.expired())
    return nullptr;

  std::shared_ptr<ExposeView> exposeView = m_exposeView;
  auto retVal = std::shared_ptr<ExposeView>(
    m_exposeView.get(),
    [exposeView] (ExposeView* pView) {
      pView->CloseView();
    }
  );
  m_weakLock = retVal;
  return retVal;
}

bool ExposeViewAccessManager::IsLocked(void) const {
  return m_weakLock.expired();
}