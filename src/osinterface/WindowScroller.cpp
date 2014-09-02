#include "stdafx.h"
#include "WindowScroller.h"

static const float MICROSECONDS_TO_SECONDS = 1E-6;

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
  const auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - m_lastScrollTimePoint).count();
  if (dt > 0) { // Just to prevent a potential divide by zero. Should never happen in practice.
    const float seconds = dt * MICROSECONDS_TO_SECONDS;

    m_VelocityX.SetGoal(deltaX/seconds);
    m_VelocityY.SetGoal(deltaY/seconds);

    m_VelocityX.Update(seconds);
    m_VelocityY.Update(seconds);
    m_lastScrollTimePoint = now;
  }
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
      m_remainingMomentum.x = MICROSECONDS_TO_SECONDS * m_VelocityX.Value();
      m_remainingMomentum.y = MICROSECONDS_TO_SECONDS * m_VelocityY.Value();
      *this += std::chrono::microseconds(16667), [this] { OnPerformMomentumScroll(); };
    }
  );
  m_curScrollOp = retVal;
  return retVal;
}

void IWindowScroller::OnPerformMomentumScroll() {
  // Do not continue performing a momentum scroll if another op is outstanding
  if(!m_curScrollOp.expired()) {
    m_remainingMomentum = OSPointZero;
    return;
  }
  const auto absMx = std::abs(m_remainingMomentum.x);
  const auto absMy = std::abs(m_remainingMomentum.y);
  if (absMy < 0.00001f && absMx < 0.00001f) {
    // Is the momentum still large enough that we want to continue?
    m_remainingMomentum = OSPointZero;
    DoScrollBy(0.0f, 0.0f, true);
    m_wse(&WindowScrollerEvents::OnScrollStopped)();
    return;
  }

  std::lock_guard<std::mutex> lk(GetLock());
  const auto now = std::chrono::steady_clock::now();
  const auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - m_lastScrollTimePoint).count();
  if (dt > 0) { // Just to prevent a potential divide by zero. Should never happen in practice.
    m_lastScrollTimePoint = now;
    const auto deltaX = m_remainingMomentum.x*dt;
    const auto deltaY = m_remainingMomentum.y*dt;
    DoScrollBy(deltaX, deltaY, true);

    // Apply drag by an exponential curve
    if (absMy < 0.0001f && absMx < 0.0001f) {
      m_remainingMomentum.x *= 0.95;
      m_remainingMomentum.y *= 0.95;
    } else {
      m_remainingMomentum.x *= 0.97;
      m_remainingMomentum.y *= 0.97;
    }
  }
  *this += std::chrono::microseconds(16667), [this] { OnPerformMomentumScroll(); };
}
