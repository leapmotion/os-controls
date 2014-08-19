#include "MediaMenuController.h"


MediaMenuController::MediaMenuController() :
m_lastProgress(std::numeric_limits<float>::min())
{
  m_mediaView = RenderEngineNode::Create<MediaView>(Vector3(300, 300, 0), 5.0f);
  m_rootNode->AddChild(m_mediaView);
}

void MediaMenuController::AutoFilter(const HandExistenceState& hes, const CursorMap& handScreenLocations, const GestureMap& handGestures) {
  if(!m_controllingHand.isValid()) { // if there is NOT a controlling hand
    if(!hes.m_newHands.empty()) {
      m_controllingHand = hes.m_newHands[0];
      m_mediaView->SetFadeState(MediaView::FADE_IN);
      
      //Enter
    }
  }
  else {
    if(!findHandInVector(hes.m_stableHands, m_controllingHand)) {
      m_controllingHand = Leap::Hand::invalid();
      m_mediaView->SetFadeState(MediaView::FADE_OUT);
      
      //Exit
      return;
    }
    
    // Update
    auto q = handGestures.find(m_controllingHand.id());
    if (q != handGestures.end()) {
      if (q->second.type() == Leap::Gesture::TYPE_CIRCLE) {
        auto circleGesture = static_cast<Leap::CircleGesture>(q->second);
        
        float direction = 1.0f;
        
        if (circleGesture.pointable().direction().angleTo(circleGesture.normal()) >= M_PI/2) {
          direction = -1.0f;
        }
        
        std::cout << "Progress Diff: " << circleGesture.progress() - m_lastProgress << std::endl;
        
        if (m_lastProgress == std::numeric_limits<float>::min() || circleGesture.progress() - m_lastProgress > 2) {
          m_lastProgress = circleGesture.progress();
          return;
        }
        
        m_mediaView->NudgeVolume(direction*(circleGesture.progress() - m_lastProgress)/10.0f);
        std::cout << "Nudge Volume: " << direction * (circleGesture.progress() - m_lastProgress)/10.0f << std::endl;
        
        m_lastProgress = circleGesture.progress();
      }
    }
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



