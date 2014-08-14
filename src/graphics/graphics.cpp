#include "stdafx.h"
#include "graphics.h"
#include "VolumeControl.h"
#include "MediaView.h"
#include "GL/glew.h"
void GraphicsInitialize(void) {
  AutoRequired<GraphicsStateMachine>();
  AutoRequired<VolumeControl>();
  AutoConstruct<MediaView>(Vector3(300, 300, 0), 5.0f);
}