#pragma once
#include "RenderEngineNode.h"

#include "Primitives.h"
#include "Color.h"

#include "Resource.h"
#include "GLShader.h"

#include <string>

class Cursor :
public RenderEngineNode
{
public:
  Cursor(float radius, const Color& color);
  
  void Move(float x, float y);
  void SetSize(float radius);
  
  void InitChildren() override;
  
  void AnimationUpdate(const RenderFrame& frame);
  void Render(const RenderFrame& frame) const override;
  
  enum FadeState {
    FADE_OUT,
    FADE_IN
  };
  void SetFadeState(FadeState newState);
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  void setOpacity(float opacity);
  
  Disk m_disk;
  
  FadeState m_fadeState;
};
