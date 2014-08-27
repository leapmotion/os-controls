#include "stdafx.h"
#include "ExposeViewAccessManager.h"
#include "graphics/ExposeView.h"

std::shared_ptr<ExposeView> ExposeViewAccessManager::Lock(void) {
  if(!m_weakLock.expired())
    return nullptr;

  auto retVal = std::make_shared<ExposeView>();
  m_weakLock = retVal;
  return retVal;
}

bool ExposeViewAccessManager::IsLocked(void) const {
  return m_weakLock.expired();
}