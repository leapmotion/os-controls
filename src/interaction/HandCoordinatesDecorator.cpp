//
//  HandCoordinatesDecorator.cpp
//  oscontrols
//
//  Created by Daniel Plemmons on 8/15/14.
//
//

#include "HandCoordinatesDecorator.h"
#include "HandPointingDecorator.h"
#include "CoordinateUtility.h"
#include "InteractionConfigs.h"

void HandCoordinatesDecorator::AutoFilter(const Leap::Frame& frame, const HandPointingMap& hpm, CursorMap& handScreenLocations) {
  for(auto hand : frame.hands()) {
    Vector2 screenLocation;
    
    screenLocation = m_coordinateUtility->LeapToScreen(hand.palmPosition());
    //If the hand is pointing, offset position with index finger.
    try {
      hpm.at(hand.id());
      for ( auto finger : hand.fingers() )
      {
        if ( finger.type() == Leap::Finger::TYPE_INDEX ) {
          Vector2 direction = ProjectVector(2, finger.direction().toVector3<Vector3>());
          direction *= config::FINGER_OFFSET_DISTANCE;
          screenLocation += direction;
        }
      }
    }
    catch (std::out_of_range e){
      //don't worry about it.
    }
    
    screenLocation.y() *= -1;
    handScreenLocations[hand.id()] = screenLocation;
  }
  //Creates a handScreenLocations reference and decorates the packet at the end of the packet.
}