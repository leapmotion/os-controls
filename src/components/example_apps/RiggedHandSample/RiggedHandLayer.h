#pragma once

#include "Primitives.h"
#include "RenderableEventHandler.h"
#include "RenderState.h"

#include "RiggedHand.h"

#include "LeapListener.h"
#include "EigenTypes.h"

#include <memory>

class GLShader;

class RiggedHandLayer : public RenderableEventHandler {
public:

  RiggedHandLayer();
  virtual ~RiggedHandLayer();

  virtual void Update(TimeDelta real_time_delta) override;
  virtual void Render(TimeDelta real_time_delta) const override;

  void SetDimensions(int width, int height) { m_Width = width; m_Height = height; }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:

  int m_Width;
  int m_Height;

  mutable RenderState m_Renderer;

  std::vector<RiggedHandRef> mRiggedHands;

  Leap::Controller mController;
  Leap::Frame mCurFrame;
  bool mPauseLeap;

  TimePoint m_time;
};
