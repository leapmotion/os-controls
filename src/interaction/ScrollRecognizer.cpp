#include "InteractionConfigs.h"
#include "ScrollRecognizer.h"

ScrollRecognizer::ScrollRecognizer() {
  m_horizontalMovementRatio.SetSmoothStrength(0.75f);
  m_horizontalMovementRatio.SetInitialValue(1.0f);

  m_scrollVelocity.SetSmoothStrength(0.5f);
  m_scrollVelocity.SetInitialValue(Vector3::Zero());

  m_curScrollVelocity.setZero();

  m_handVelocity.setZero();
  m_handDirection = -Vector3::UnitZ();
  m_handNormal = -Vector3::UnitY();

  m_prevTimestamp = 0;
  m_deltaTimeSeconds = 0.01f;
}

void ScrollRecognizer::AutoFilter(const Leap::Hand& hand, Scroll& scroll) {
  m_hand = hand;

  ExtractFrameData();
  UpdateHorizontalMovementRatio();

  AccumulateScrollFromFingers();
  UpdateScrollVelocity();
  scroll.m_deltaScrollMM = m_scrollVelocity.Value() * m_deltaTimeSeconds;
}

void ScrollRecognizer::ExtractFrameData() {
  // calculate delta time
  static const float MICROSECONDS_TO_SECONDS = 1E-6;
  const Leap::Frame curFrame = m_hand.frame();
  const int64_t curTimestamp = curFrame.timestamp();
  m_deltaTimeSeconds = static_cast<float>(MICROSECONDS_TO_SECONDS * (curTimestamp - m_prevTimestamp));
  m_prevTimestamp = curTimestamp;

  // retrieve hand data
  m_handVelocity = m_hand.palmVelocity().toVector3<Vector3>();
  m_handDirection = m_hand.direction().toVector3<Vector3>();
  m_handNormal = m_hand.palmNormal().toVector3<Vector3>();
}

void ScrollRecognizer::UpdateHorizontalMovementRatio() {
  const Vector3 normVelocity = m_handVelocity.normalized();
  const float ratio = static_cast<float>(normVelocity.x()*normVelocity.x() + normVelocity.y()*normVelocity.y());
  m_horizontalMovementRatio.SetGoal(ratio);
  m_horizontalMovementRatio.Update(m_deltaTimeSeconds);
}

float ScrollRecognizer::ComputeWarmupMultiplier() const {
  static float HAND_WARMUP_TIME = 2.0f;
  const float warmupRatio = std::min(1.0f, m_hand.timeVisible() / HAND_WARMUP_TIME);
  return (warmupRatio * warmupRatio);
}

Vector3 ScrollRecognizer::ComputeRoundedHandVelocity() const {
  const float ratio = m_horizontalMovementRatio.Value();
  return Vector3(ratio*m_handVelocity.x(), ratio*m_handVelocity.y(), (1.0-ratio)*m_handVelocity.z());
}

void ScrollRecognizer::UpdateScrollVelocity() {
  // make speeding up have less lag than slowing down
  static const float SPEED_UP_SMOOTH = 0.4f;
  static const float SLOW_DOWN_SMOOTH = 0.7f;
  static const float DECAY_MULTIPLIER = 0.6f;

  const Vector3 prevScrollVelocity = m_scrollVelocity.Value();

  const float curSmooth = m_curScrollVelocity.squaredNorm() > prevScrollVelocity.squaredNorm() ? SPEED_UP_SMOOTH : SLOW_DOWN_SMOOTH;
  m_scrollVelocity.SetSmoothStrength(curSmooth);
  m_scrollVelocity.SetGoal(m_curScrollVelocity + DECAY_MULTIPLIER*prevScrollVelocity);
  m_scrollVelocity.Update(m_deltaTimeSeconds);
}

void ScrollRecognizer::AccumulateScrollFromFingers() {
  m_curScrollVelocity.setZero();

  const Vector3 roundedVelocity = ComputeRoundedHandVelocity();

  const Leap::FingerList fingers = m_hand.fingers();

  for (int i=0; i<fingers.count(); i++) {
    const Leap::Finger finger = fingers[i];
    if (!finger.isExtended()) {
      // ignore fingers that are tucked in
      continue;
    }

    const Vector3 direction = finger.direction().toVector3<Vector3>();
    const Vector3 velocity = finger.tipVelocity().toVector3<Vector3>();
    const Vector3 normVelocity = velocity.normalized();

    const double velocityDotHandNormal = std::abs(normVelocity.dot(m_handNormal));
    const double directionDotHandDirection = direction.dot(m_handDirection);
    const double velocityMult = DeadZoneMultiplier(velocity);

    m_curScrollVelocity += velocityMult * velocityDotHandNormal * directionDotHandDirection * roundedVelocity;
  }

  m_curScrollVelocity *= ComputeWarmupMultiplier();
}

float ScrollRecognizer::DeadZoneMultiplier(const Vector3& velocity) {
  static const float LOW_WATERMARK = 200.0f; // multiplier is 0 below this
  static const float HIGH_WATERMARK = 500.0f; // multiplier is 1 above this
  const float ratio = static_cast<float>(velocity.norm() - LOW_WATERMARK) / (HIGH_WATERMARK - LOW_WATERMARK);
  const float multiplier = std::max(0.0f, std::min(1.0f, ratio));
  return multiplier;
}
