#include "MediaControlLayer.h"

#include <cmath>

#include "GLShaderLoader.h"
#include "GLTexture2Loader.h"
#include "Leap/GL/GLShader.h"
#include "Leap/GL/GLTexture2.h"
#include "Resource.h"

const Rgba<float> bgColor(0.4f, 0.425f, 0.45f, 0.75f);
const Rgba<float> fillColor(0.4f, 0.8f, 0.4f, 0.7f);
const Rgba<float> handleColor(0.65f, 0.675f, 0.7f, 1.0f);
const Rgba<float> handleOutlineColor(0.6f, 1.0f, 0.6f, 1.0f);

MediaControlLayer::MediaControlLayer() :
  m_Width(640),
  m_Height(480),
  m_time(0)
{
  const double startAngle = -M_PI/4.0;
  const double endAngle = startAngle + 1.5 * M_PI;

  const double volumeRadius = 8.625;
  const double volumeThickness = 0.75;

  m_shader = Resource<GLShader>("material");
  m_shader->CheckForTypedAttribute("position", GL_FLOAT_VEC3, VariableIs::OPTIONAL_BUT_WARN);
  m_shader->CheckForTypedAttribute("normal", GL_FLOAT_VEC3, VariableIs::OPTIONAL_BUT_WARN);
  m_shader->CheckForTypedAttribute("tex_coord", GL_FLOAT_VEC2, VariableIs::OPTIONAL_BUT_WARN);

  m_VolumeSlider.SetRadius(volumeRadius);
  m_VolumeSlider.SetThickness(volumeThickness);
  m_VolumeSlider.SetStartAngle(startAngle);
  m_VolumeSlider.SetEndAngle(endAngle);
  m_VolumeSlider.SetFillColor(fillColor);
  m_VolumeSlider.SetHandleColor(handleColor);
  m_VolumeSlider.SetHandleOutlineColor(handleOutlineColor); 
  m_VolumeSlider.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = bgColor;

  const int numItems = 3;
  m_PlaybackMenu.SetStartAngle(startAngle);
  m_PlaybackMenu.SetEndAngle(endAngle);
  m_PlaybackMenu.SetNumItems(numItems);
  for (int i=0; i<numItems; i++) {
    std::shared_ptr<RadialMenuItem>& item = m_PlaybackMenu.GetItem(i);
    item->SetRadius(13.5);
    item->SetThickness(7.0);
    item->SetActivatedRadius(17.0);
    item->Material().Uniform<DIFFUSE_LIGHT_COLOR>() = bgColor;
    item->SetHoverColor(fillColor);
    item->SetActivatedColor(handleOutlineColor);
  }

  Resource<TextFile> nextIconFile("next-track-icon-extended-01.svg");
  Resource<TextFile> playPauseIconFile("play_pause-icon-extended-01.svg");
  Resource<TextFile> prevIconFile("prev-track-icon-extended-01.svg");
  Resource<TextFile> volumeIconFile("volume-icon-01.svg");

  std::shared_ptr<SVGPrimitive> nextIcon(new SVGPrimitive());
  std::shared_ptr<SVGPrimitive> playPauseIcon(new SVGPrimitive());
  std::shared_ptr<SVGPrimitive> prevIcon(new SVGPrimitive());
  std::shared_ptr<SVGPrimitive> volumeIcon(new SVGPrimitive());

  nextIcon->Set(nextIconFile->Contents());
  playPauseIcon->Set(playPauseIconFile->Contents());
  prevIcon->Set(prevIconFile->Contents());
  volumeIcon->Set(volumeIconFile->Contents());

  m_PlaybackMenu.GetItem(0)->SetIcon(nextIcon);
  m_PlaybackMenu.GetItem(1)->SetIcon(playPauseIcon);
  m_PlaybackMenu.GetItem(2)->SetIcon(prevIcon);
  m_VolumeSlider.SetIcon(volumeIcon);

  m_Controller.addListener(m_Listener);

  m_Cursor.SetRadius(2.0);

  m_Volume = 0.5;
}

MediaControlLayer::~MediaControlLayer() {

}

void MediaControlLayer::Update(TimeDelta real_time_delta) {
  m_time += real_time_delta;

  std::deque<Leap::Frame> frames = m_Listener.TakeAccumulatedFrames();

  // update volume
#if 0
  m_Volume = 0.5 * (1.0 + std::sin(0.5*m_time));
#else
  m_VolumeSlider.SetFillColor(handleColor);
  m_VolumeSlider.SetHandleOutlineColor(handleColor);
  static Leap::Frame prevFrame;
  if (!frames.empty()) {
    Leap::Frame curFrame = frames.back();

    if (curFrame.isValid() && prevFrame.isValid()) {
      Leap::GestureList gestures = curFrame.gestures(prevFrame);
      for (int i=0; i<gestures.count(); i++) {
        if (gestures[i].type() == Leap::Gesture::TYPE_CIRCLE && gestures[i].state() == Leap::Gesture::STATE_UPDATE) {
          // draw circle gesture
          Leap::CircleGesture circle = gestures[i];

          const bool clockwise = circle.normal().z > 0;

          const double updateVelocity = 0.2;
          const double updateAmt = updateVelocity *real_time_delta;
          if (clockwise) {
            m_Volume += updateAmt;
            if (m_Volume > 1.0) {
              m_Volume = 1.0;
            }
          } else {
            m_Volume -= updateAmt;
            if (m_Volume < 0.0) {
              m_Volume = 0.0;
            }
          }
          m_VolumeSlider.SetFillColor(fillColor);
          m_VolumeSlider.SetHandleOutlineColor(handleOutlineColor);
        }
      }

      Leap::HandList hands = curFrame.hands();
#if 1
#else
      m_HandCursors.resize(hands.count());
      for (int i=0; i<hands.count(); i++) {
        m_HandCursors[i].Update(hands[i]);
      }
#endif
    }

    prevFrame = curFrame;
  }
#endif

  // update menu items
#if 0
  const double timeMod = std::fmod(m_time, 3.0);
  double item1Activation = 0;
  double item2Activation = 0;
  double item3Activation = 0;
  if (timeMod <= 1.0) {
    item1Activation = timeMod;
  } else if (timeMod > 1.0 && timeMod <= 2.0) {
    item2Activation = timeMod - 1.0;
  } else {
    item3Activation = timeMod - 2.0;
  }
  m_PlaybackMenu.GetItem(0)->SetActivation(item1Activation);
  m_PlaybackMenu.GetItem(1)->SetActivation(item2Activation);
  m_PlaybackMenu.GetItem(2)->SetActivation(item3Activation);
#else
  static const EigenTypes::Vector3 LEAP_OFFSET(0, -200, 0);
  static const double LEAP_SCALE = 0.5;
  if (!frames.empty()) {
    Leap::Finger frontmostFinger = frames.back().fingers().frontmost();
    const EigenTypes::Vector3 fingerPos = frontmostFinger.tipPosition().toVector3<EigenTypes::Vector3>() + LEAP_OFFSET;
    m_Cursor.Translation() = LEAP_SCALE * fingerPos;
    m_Cursor.Translation().z() = 0.0;
  }
  assert(false && "this seems to be an API break from RadialMenu.  TODO: correct this.");
  // m_PlaybackMenu.UpdateItemsFromCursor(m_Cursor.Translation(), static_cast<float>(real_time_delta));
#endif
}

void MediaControlLayer::Render(TimeDelta real_time_delta) const {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // set renderer projection matrix
  const double fovRadians = (M_PI / 180.0) * 60;
  const double widthOverHeight = static_cast<double>(m_Width)/static_cast<double>(m_Height);
  const double nearClip = 1.0;
  const double farClip = 10000.0; 
  m_Renderer.GetProjection().Perspective(fovRadians, widthOverHeight, nearClip, farClip);

  // set renderer modelview matrix
  const EigenTypes::Vector3 eyePos = 100*EigenTypes::Vector3::UnitZ(); // + 0.5*m_Cursor.Translation();
  const EigenTypes::Vector3 lookAtPoint = EigenTypes::Vector3::Zero();
  const EigenTypes::Vector3 upVector = EigenTypes::Vector3::UnitY();
  //m_Renderer.SetShader(m_shader);
  m_Renderer.GetModelView().Reset();
  m_Renderer.GetModelView().LookAt(eyePos, lookAtPoint, upVector);

  m_shader->Bind();

  // set light position
  const EigenTypes::Vector3f desiredLightPos(0, 10, 10);
  const EigenTypes::Vector3f lightPos = desiredLightPos - eyePos.cast<float>();
  const int lightPosLoc = m_shader->LocationOfUniform("lightPosition");
  glUniform3f(lightPosLoc, lightPos[0], lightPos[1], lightPos[2]);

  PrimitiveBase::DrawSceneGraph(m_PlaybackMenu, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_VolumeSlider, m_Renderer);
#if 1
  PrimitiveBase::DrawSceneGraph(m_Cursor, m_Renderer);
#else
  for (size_t i=0; i<m_HandCursors.size(); i++) {
    PrimitiveBase::DrawSceneGraph(m_HandCursors[i], m_Renderer);
  }
#endif

  m_shader->Unbind();
}
