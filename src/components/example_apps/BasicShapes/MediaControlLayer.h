#pragma once

#include "Primitives.h"
#include "RenderableEventHandler.h"
#include "RenderState.h"

#include <memory>

class GLShader;
class GLTexture2;

class MediaControlLayer : public RenderableEventHandler {
public:

  MediaControlLayer();
  virtual ~MediaControlLayer();

  virtual void Update(TimeDelta real_time_delta) override;
  virtual void Render(TimeDelta real_time_delta) const override;

  void SetDimensions(int width, int height) { m_Width = width; m_Height = height; }

private:

  int m_Width;
  int m_Height;

  PartialDisk m_LeftSection;
  PartialDisk m_TopSection;
  PartialDisk m_RightSection;

  PartialDisk m_VolumeFill;
  PartialDisk m_VolumeTrack;
  Disk m_VolumeHandle;
  Disk m_VolumeHandleOutline;

  std::shared_ptr<GLTexture2> m_RewindTexture;
  std::shared_ptr<GLTexture2> m_PlayPauseTexture;
  std::shared_ptr<GLTexture2> m_FastForwardTexture;
  RectanglePrim m_Rewind;
  RectanglePrim m_PlayPause;
  RectanglePrim m_FastForward;

  double m_Volume;

  mutable RenderState m_Renderer;

  std::shared_ptr<GLShader> m_shader;
  TimePoint m_time;
};
