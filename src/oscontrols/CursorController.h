#pragma once

#include "interaction/HandExistTrigger.h"
#include "interaction/HandLocationRecognizer.h"
#include "graphics/Cursor.h"
#include "graphics/RenderEngine.h"
#include "Leap.h"

#include <autowiring/autowiring.h>


class CursorController {
public:
  CursorController(void);
  
  void AutoFilter(const Leap::Hand& hand, const HandLocation& handScreenLocations);
  
private:
  
  bool findHandInVector(std::vector<Leap::Hand> vect, Leap::Hand goalHand);
  
  Leap::Hand m_controllingHand;
  
  std::shared_ptr<Cursor> m_cursor;
  AutoRequired<RenderEngineNode> m_rootNode;
};