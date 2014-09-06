#pragma once

#include "Animation.h"
#include "uiEvents/Updatable.h"
#include <Primitives.h>

class VolumeKnob :
  public Updatable,
  public PrimitiveBase {
public:
  VolumeKnob();
  virtual ~VolumeKnob();
  
  void Tick(std::chrono::duration<double> deltaT) override;
  void SetAlphaMaskGoal(float alphaMaskGoal);

protected:

  void DrawContents(RenderState &render_state) const override;

private:

  void AddChildrenIfNecessary();

  bool m_childrenAdded;
  std::shared_ptr<Disk> m_knobBody;
  std::shared_ptr<RectanglePrim> m_knobIndicator;
  Smoothed<float> m_alphaMask;
};