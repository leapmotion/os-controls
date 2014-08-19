#pragma once

#include "interaction/HandExistTrigger.h"
#include "interaction/CircleGestureTrigger.h"
#include "interaction/HandCoordinatesDecorator.h"
#include "graphics/MediaView.h"
#include "graphics/RenderEngine.h"
#include "SceneGraphNode.h"
#include "Leap.h"
#include "osinterface/AudioVolumeInterface.h"
#include "osinterface/MediaInterface.h"

#include <autowiring/autowiring.h>


class MediaMenuController {
public:
  MediaMenuController(void);
  void AutoFilter(const HandPointingMap &hpm, const CursorMap& handScreenLocations, const GestureMap& handGestures);
  
private:
  float m_lastProgress;
  
private:
  void closeMenu(bool keepSelectionVisible);
  void updateVolumeControl(const GestureMap& handGestures);
  void updateWedges(const HandPointingMap &hpm, const CursorMap& handScreenLocations);

  Leap::Hand m_controllingHand;
  bool m_isInteractionComplete; // Do we need to wait for a new hand to be activated 

  std::shared_ptr<MediaView> m_mediaView;
  AutoRequired<RenderEngineNode> m_rootNode;
  AutoRequired<AudioVolumeInterface> m_audioVolumeInterface;
  AutoRequired<MediaInterface> m_mediaInterface;
};