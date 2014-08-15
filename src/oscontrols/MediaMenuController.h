#pragma once

#include "interaction/HandExistTrigger.h"
#include "graphics/MediaView.h"
#include "graphics/RenderEngine.h"
#include "Leap.h"

#include <autowiring/autowiring.h>


class MediaMenuController {
public:
  MediaMenuController(void);
  
  void AutoFilter(const HandExistenceState& hes);
  
private:
  enum FadeState {
    FADE_OUT,
    FADE_IN
  };
  
  bool findHandInVector(std::vector<Leap::Hand> vect, Leap::Hand goalHand);
  
  FadeState m_fadeState;

  AutoRequired<RenderEngineNode> m_rootNode;
  
  std::shared_ptr<MediaView> m_mediaView;
  
  Leap::Hand m_controllingHand;
};