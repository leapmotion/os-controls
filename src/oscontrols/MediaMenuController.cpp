#include "MediaMenuController.h"


MediaMenuController::MediaMenuController()
{
  m_mediaView = RenderEngineNode::Create<MediaView>(Vector3(300, 300, 0), 5.0f);
  m_rootNode->AddChild(m_mediaView);
}

void MediaMenuController::AutoFilter(const HandExistenceState &hes) {
  if(!m_controllingHand.isValid()) { // if there is NOT a controlling hand
    if(!hes.m_newHands.empty()) {
      m_controllingHand = hes.m_newHands[0];
      m_mediaView->SetFadeState(MediaView::FADE_IN);
      std::cout << "set new controlling hand" << std::endl;
    }
  }
  else if(!findHandInVector(hes.m_stableHands, m_controllingHand)) {
    m_mediaView->SetFadeState(MediaView::FADE_OUT); 
    m_controllingHand = Leap::Hand::invalid();
    std::cout << "remove controlling hand" << std::endl;
  }
}

bool MediaMenuController::findHandInVector(std::vector<Leap::Hand> vect, Leap::Hand goalHand) {
  bool retVal = false;
  for(Leap::Hand hand : vect) {
    if(hand.id() == goalHand.id()) {
      retVal = true;
    }
  }
  return retVal;
}



