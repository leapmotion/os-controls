#include "stdafx.h"
#include "ExposeViewWindow.h"

ExposeViewWindow::ExposeViewWindow(void) {}
ExposeViewWindow::~ExposeViewWindow(void) {}

void ExposeViewWindow::SetActivation(float activation) {
  m_activation = activation;
}

std::shared_ptr<ExposeViewWindow::ExposeViewWindowActivation> ExposeViewWindow::LockActivation(void) {
  if(!m_activationWeak.expired())
    return nullptr;

  auto retVal = std::shared_ptr<ExposeViewWindowActivation>(
    static_cast<ExposeViewWindowActivation*>(this),
    [] (ExposeViewWindowActivation* pWindow) {
      pWindow->SetActivation(0.0);
    }
  );
  m_activationWeak = retVal;
  return retVal;
}
