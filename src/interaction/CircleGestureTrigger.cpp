//
//  CircleGestureTrigger.cpp
//  oscontrols
//
//  Created by Paul Mandel on 8/18/14.
//
//

#include "CircleGestureTrigger.h"
#include "CoordinateUtility.h"


void CircleGestureTrigger::AutoFilter(const Leap::Frame& frame, GestureMap& handGestures) {
  Leap::GestureList gestures = frame.gestures();
  
  for(auto hand : frame.hands()) {
    for(Leap::GestureList::const_iterator gl = gestures.begin(); gl != gestures.end(); gl++) {
      if ((*gl).type() == Leap::Gesture::TYPE_CIRCLE) {
        for(auto gesture_hand : (*gl).hands()) {
          if (gesture_hand == hand) {
            handGestures[hand.id()] = (*gl);
          }
        }
      }
    }
  }
  //Creates a handScreenLocations reference and decorates the packet at the end of the packet.
}