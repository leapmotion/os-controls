#include "stdafx.h"
#include "WindowScroller.h"

IWindowScroller::IWindowScroller(void):
  m_virtualPosition(OSPointZero),
  m_remainingMomentumX(0.0f),
  m_remainingMomentumY(0.0f),
  m_scrollPartialPixel(OSPointZero),
  m_scrollPartialLine(OSPointZero),
  m_pixelsPerLine(OSPointMake(1.0f, 1.0f))
{}

IWindowScroller::~IWindowScroller(void) {}

void IWindowScroller::ScrollBy(const OSPoint& virtualPosition, float deltaX, float deltaY) {
  m_virtualPosition = virtualPosition;
  DoScrollBy(deltaX, deltaY, false);
}

void IWindowScroller::CancelScroll(void) {
  // Fix the scroll momentum at zero
  m_remainingMomentumX = 0.0f;
  m_remainingMomentumY = 0.0f;

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
    m_remainingMomentumX = 0.0f;
    m_remainingMomentumY = 0.0f;
    return;
  }

  std::lock_guard<std::mutex> lk(GetLock());
  auto now = std::chrono::high_resolution_clock::now();

  // Find out how much time has passed, and compute a scroll amount
  auto dt = then - now;
  DoScrollBy(m_remainingMomentumX * dt.count(), m_remainingMomentumY * dt.count(), true);

  // Apply drag by an exponential curve
  m_remainingMomentumX *= 0.75f;
  m_remainingMomentumY *= 0.75f;

  // Is the momentum still large enough that we want to continue?
  if(m_remainingMomentumX < 0.1f && m_remainingMomentumY < 0.1f) {
    m_remainingMomentumX = 0.0f;
    m_remainingMomentumY = 0.0f;
    m_wse(&WindowScrollerEvents::OnScrollStopped)();
  }
  else
    *this += std::chrono::milliseconds(10), [this, now] { OnPerformMomentumScroll(now); };
}
