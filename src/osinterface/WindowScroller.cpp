#include "stdafx.h"
#include "WindowScroller.h"

IWindowScroller::IWindowScroller(void):
  scrollX(0),
  scrollY(0),
  remainingMomentumX(0.0),
  remainingMomentumY(0.0)
{}

IWindowScroller::~IWindowScroller(void) {}

void IWindowScroller::ScrollBy(uint32_t virtualX, uint32_t virtualY, double unitsX, double unitsY) {
  scrollX = virtualX;
  scrollY = virtualY;
  DoScrollBy(virtualX, virtualY, unitsX, unitsY);
}

void IWindowScroller::CancelScroll(void) {
  // Fix the scroll momentum at zero
  remainingMomentumX = 0.0;
  remainingMomentumY = 0.0;

  // We don't care to retain the weak pointer anymore
  m_curScrollOp.reset();
}

std::shared_ptr<IScrollOperation> IWindowScroller::BeginScroll(void) {
  std::lock_guard<std::mutex> lk(GetLock());
  if(!m_curScrollOp.expired())
    return nullptr;

  auto retVal = std::shared_ptr<IScrollOperation>(
    static_cast<IScrollOperation*>(this),
    [this] (IScrollOperation*) {
      *this += [this] { OnPerformMomentumScroll(std::chrono::high_resolution_clock::now()); };
    }
  );
  m_curScrollOp = retVal;
  return retVal;
}

void IWindowScroller::OnPerformMomentumScroll(std::chrono::high_resolution_clock::time_point then) {
  // Do not continue performing a momentum scroll if another op is outstanding
  if(!m_curScrollOp.expired()) {
    remainingMomentumX = 0.0;
    remainingMomentumY = 0.0;
    return;
  }

  std::lock_guard<std::mutex> lk(GetLock());
  auto now = std::chrono::high_resolution_clock::now();

  // Find out how much time has passed, and compute a scroll amount
  auto dt = then - now;
  DoScrollBy(scrollX, scrollY, remainingMomentumX * dt.count(), remainingMomentumY * dt.count());

  // Apply drag by an exponential curve
  remainingMomentumX *= 0.75;
  remainingMomentumY *= 0.75;

  // Is the momentum still large enough that we want to continue?
  if(remainingMomentumX < 0.1 && remainingMomentumY < 0.1) {
    remainingMomentumX = 0.0;
    remainingMomentumY = 0.0;
    m_wse(&WindowScrollerEvents::OnScrollStopped)();
  }
  else
    *this += std::chrono::milliseconds(10), [this, now] { OnPerformMomentumScroll(now); };
}