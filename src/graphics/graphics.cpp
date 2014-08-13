#include "stdafx.h"
#include "graphics.h"
#include "GraphicsStateMachine.h"
#include "VolumeControl.h"
#include "GL/glew.h"
void GraphicsInitialize(void) {
  AutoRequired<GraphicsStateMachine>();
  AutoRequired<VolumeControl>();
}