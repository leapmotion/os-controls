#include "CursorController.h"


CursorController::CursorController()
{
  m_cursor = RenderEngineNode::Create<Cursor>(10.0f, Color(0.3486f, 0.573f, 0.0784f));
  m_rootNode->AddChild(m_cursor);
}

void CursorController::AutoFilter(const HandExistenceState& hes, const CursorMap& handScreenLocations) {
  if(!m_controllingHand.isValid()) { // if there is NOT a controlling hand
    if(!hes.m_newHands.empty()) {
      m_controllingHand = hes.m_newHands[0];
      m_cursor->SetFadeState(Cursor::FADE_IN);
      m_cursor->Move(300, 300);
    }
  }
  else {
    if(!findHandInVector(hes.m_stableHands, m_controllingHand)) {
      m_controllingHand = Leap::Hand::invalid();
      m_cursor->SetFadeState(Cursor::FADE_OUT);
      return;
    }
    
    auto q = handScreenLocations.find(m_controllingHand.id());
    if (q != handScreenLocations.end()) {
      m_cursor->Move(q->second.x(), -q->second.y());
    }
  }
}

bool CursorController::findHandInVector(std::vector<Leap::Hand> vect, Leap::Hand goalHand) {
  bool retVal = false;
  for(Leap::Hand hand : vect) {
    if(hand.id() == goalHand.id()) {
      retVal = true;
    }
  }
  return retVal;
}



