#pragma once

#include "interaction/HandExistTrigger.h"
#include "interaction/CircleGestureTrigger.h"
#include "interaction/HandCoordinatesDecorator.h"
#include "graphics/MediaView.h"
#include "graphics/RenderEngine.h"
#include "Leap.h"
#include "osinterface/AudioVolumeInterface.h"

#include <autowiring/autowiring.h>


class MediaMenuController {
public:
  MediaMenuController(void);
  
  void AutoFilter(const HandExistenceState& hes, const CursorMap& handScreenLocations, const GestureMap& handGestures);
  
private:
  bool findHandInVector(std::vector<Leap::Hand> vect, Leap::Hand goalHand);

  Leap::Hand m_controllingHand;
  
  float m_lastProgress;

  std::shared_ptr<MediaView> m_mediaView;
  AutoRequired<RenderEngineNode> m_rootNode;
  
  AutoRequired<AudioVolumeInterface> m_audioVolumeInterface;
};