#include "stdafx.h"
#include "ExposeViewAccessManager.h"
#include "ExposeView.h"
#include "ExposeViewController.h"

#include "osinterface/OSWindowMonitor.h"

ExposeViewAccessManager::ExposeViewAccessManager(void):
  m_exposeView(ctxt),
  m_exposeViewController(ctxt)
{
  ctxt->Initiate();
}

std::shared_ptr<ExposeView> ExposeViewAccessManager::Lock(void) {
  if(!m_weakLock.expired())
    return nullptr;

  AutowiredFast<OSWindowMonitor> oswm;

  // Create an intentionally alised shared pointer so we can keep track of this request
  std::shared_ptr<ExposeView> exposeView = m_exposeView;
  auto retVal = std::shared_ptr<ExposeView>(
    m_exposeView.get(),
    [exposeView, oswm] (ExposeView* pView) {
      pView->CloseView();
      oswm->EnableScan(false);
    }
  );
  m_weakLock = retVal;
  oswm->EnableScan(true);
  return retVal;
}

bool ExposeViewAccessManager::IsLocked(void) const {
  return m_weakLock.expired();
}