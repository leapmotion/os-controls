//
//  HandCoordinatesDecorator.cpp
//  oscontrols
//
//  Created by Daniel Plemmons on 8/15/14.
//
//

#include "HandCoordinatesDecorator.h"
#include "CoordinateUtility.h"


void HandCoordinatesDecorator::AutoFilter(const Leap::Frame& frame, CursorMap& handScreenLocations) {
  for(auto hand : frame.hands()) {
    const Vector2 screenLocation = m_coordinateUtility->LeapToScreen(hand.palmPosition());
    handScreenLocations[hand.id()] = screenLocation;
  }
  //Creates a handScreenLocations reference and decorates the packet at the end of the packet.
}