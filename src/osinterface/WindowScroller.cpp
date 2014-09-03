#include "stdafx.h"
#include "WindowScroller.h"

IWindowScroller::IWindowScroller(void):
  m_virtualPosition(OSPointZero),
  m_ppmm(110.0f/25.4f), // Determine this dynamically -- FIXME
  m_remainingMomentum(OSPointZero),
  m_lastScrollTimePoint(std::chrono::steady_clock::now())
{
  m_VelocityX.SetInitialValue(0.0f);
  m_VelocityY.SetInitialValue(0.0f);
  m_VelocityX.SetSmoothStrength(0.5f);
  m_VelocityY.SetSmoothStrength(0.5f);
}

IWindowScroller::~IWindowScroller(void)
{}

void IWindowScroller::ScrollBy(const OSPoint& virtualPosition, float deltaX, float deltaY) {
  std::lock_guard<std::mutex> lk(GetLock());
  m_virtualPosition = virtualPosition;

  const auto now = std::chrono::steady_clock::now();
  const std::chrono::duration<double> dt = now - m_lastScrollTimePoint;
  const float seconds = (float)dt.count();
  if (seconds > 0) { // Just to prevent a potential divide by zero. Should never happen in practice.
    m_VelocityX.SetGoal(deltaX/seconds);
    m_VelocityY.SetGoal(deltaY/seconds);

    m_VelocityX.Update(seconds);
    m_VelocityY.Update(seconds);
    m_lastScrollTimePoint = now;
  }
  DoScrollBy(deltaX, deltaY, false);
}

void IWindowScroller::CancelScroll(void) {
  std::lock_guard<std::mutex> lk(GetLock());
  if (std::abs(m_remainingMomentum.y) >= FLT_EPSILON || std::abs(m_remainingMomentum.x) >= FLT_EPSILON) {
    m_remainingMomentum = OSPointZero;
    DoScrollBy(0.0f, 0.0f, true);
  }
  m_wse(&WindowScrollerEvents::OnScrollStopped)();

  // We don't care to retain the weak pointer anymore
  m_curScrollOp.reset();
}

std::shared_ptr<IScrollOperation> IWindowScroller::BeginScroll(void) {
  std::lock_guard<std::mutex> lk(GetLock());
  if (!m_curScrollOp.expired()) {
    return nullptr;
  }
  if (std::abs(m_remainingMomentum.y) >= FLT_EPSILON || std::abs(m_remainingMomentum.x) >= FLT_EPSILON) {
    m_remainingMomentum = OSPointZero;
    DoScrollBy(0.0f, 0.0f, true);
    m_wse(&WindowScrollerEvents::OnScrollStopped)();
  }

  auto retVal = std::shared_ptr<IScrollOperation>(
    static_cast<IScrollOperation*>(this),
    [this] (IScrollOperation*) {
      std::lock_guard<std::mutex> lk(GetLock());
      m_remainingMomentum.x = m_VelocityX.Value();
      m_remainingMomentum.y = m_VelocityY.Value();
      m_VelocityX.SetInitialValue(0.0f);
      m_VelocityY.SetInitialValue(0.0f);
      *this += std::chrono::microseconds(16667), [this] { OnPerformMomentumScroll(); };
    }
  );
  m_curScrollOp = retVal;
  return retVal;
}

void IWindowScroller::OnPerformMomentumScroll() {
  std::lock_guard<std::mutex> lk(GetLock());

  // Do not continue performing a momentum scroll if another op is outstanding
  if (!m_curScrollOp.expired()) {
    return;
  }
  const auto absMx = std::abs(m_remainingMomentum.x);
  const auto absMy = std::abs(m_remainingMomentum.y);

  // Is the momentum still large enough that we want to continue?
  if (absMy < 1e-9f && absMx < 1e-9f) {
    m_remainingMomentum = OSPointZero;
    DoScrollBy(0.0f, 0.0f, true);
    m_wse(&WindowScrollerEvents::OnScrollStopped)();
    return;
  }

  const auto now = std::chrono::steady_clock::now();
  const std::chrono::duration<double> dt = now - m_lastScrollTimePoint;

  // Queue up the next scroll:
  *this += std::chrono::microseconds(16667), [this] { OnPerformMomentumScroll(); };
  m_lastScrollTimePoint = now;

  const float seconds = (float)dt.count();
  if (seconds <= 0.0f)
    // Just to prevent a potential divide by zero. Should never happen in practice.
    return;

  const auto deltaX = m_remainingMomentum.x*seconds;
  const auto deltaY = m_remainingMomentum.y*seconds;
  DoScrollBy(deltaX, deltaY, true);

  // Apply drag by an exponential curve
  if (absMy < 0.0001f && absMx < 0.0001f) {
    m_remainingMomentum.x *= 0.94f;
    m_remainingMomentum.y *= 0.94f;
  } else {
    m_remainingMomentum.x *= 0.97f;
    m_remainingMomentum.y *= 0.97f;
  }
}

void IWindowScroller::StopMomentumScrolling(void) {
  std::lock_guard<std::mutex>(GetLock()),
  m_curScrollOp.reset();
}