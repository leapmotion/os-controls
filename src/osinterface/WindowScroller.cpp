#include "stdafx.h"
#include "WindowScroller.h"

IWindowScroller::IWindowScroller(void):
  m_virtualPosition(OSPointZero),
  m_remainingMomentum(OSPointZero),
  m_scrollPartialPixel(OSPointZero),
  m_scrollPartialLine(OSPointZero),
  m_pixelsPerLine(OSPointMake(1.0f, 1.0f))
{}

IWindowScroller::~IWindowScroller(void) {}

void IWindowScroller::ScrollBy(const OSPoint& virtualPosition, float deltaX, float deltaY) {
  std::lock_guard<std::mutex> lk(GetLock());
  m_virtualPosition = virtualPosition;
  DoScrollBy(deltaX, deltaY, false);
}

void IWindowScroller::CancelScroll(void) {
  // Fix the scroll momentum at zero
  m_remainingMomentum = OSPointZero;

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
    m_remainingMomentum = OSPointZero;
    return;
  }

  std::lock_guard<std::mutex> lk(GetLock());
  auto now = std::chrono::high_resolution_clock::now();

  // Find out how much time has passed, and compute a scroll amount
  auto dt = then - now;
  DoScrollBy(m_remainingMomentum.x * dt.count(), m_remainingMomentum.y * dt.count(), true);

  // Apply drag by an exponential curve
  m_remainingMomentum.x *= 0.75f;
  m_remainingMomentum.y *= 0.75f;

  // Is the momentum still large enough that we want to continue?
  if(m_remainingMomentum.x < 0.1f && m_remainingMomentum.y < 0.1f) {
    m_remainingMomentum.x = 0.0f;
    m_remainingMomentum.y = 0.0f;
    m_wse(&WindowScrollerEvents::OnScrollStopped)();
  }
  else
    *this += std::chrono::milliseconds(10), [this, now] { OnPerformMomentumScroll(now); };
}
