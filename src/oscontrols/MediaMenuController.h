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
  
  bool findHandInVector(std::vector<Leap::Hand> vect, Leap::Hand goalHand);

  Leap::Hand m_controllingHand;

  std::shared_ptr<MediaView> m_mediaView;
  AutoRequired<RenderEngineNode> m_rootNode;
};