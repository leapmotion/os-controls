#pragma once

#include "interaction/HandExistTrigger.h"
#include "graphics/MediaView.h"
#include "graphics/RenderEngine.h"
#include "Leap.h"

#include <autowiring/autowiring.h>


class MediaMenuController {
public:
  MediaMenuController(void);
  
private:
  AutoRequired<RenderEngineNode> m_rootNode;
  
  std::shared_ptr<MediaView> m_mediaView;
  
  Leap::Hand m_controllingHand;
  
  void AutoFilter(const HandExistenceState& hes);
  
  
  bool findHandInVector(std::vector<Leap::Hand> vect, Leap::Hand goalHand);
};