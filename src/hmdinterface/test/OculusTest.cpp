// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "hmdinterface/OculusRift/RiftContext.h"
#include "hmdinterface/OculusRift/RiftDevice.h"
#include "osinterface/RenderWindow.h"

class OculusTest :
public testing::Test
{};

TEST_F(OculusTest, BasicSquare) {
  std::shared_ptr<RenderWindow> window1(RenderWindow::New());
  window1->SetRect({ 300, 300, 640, 480 });
  window1->SetTransparent(false);
  window1->SetVisible(true);



    hmdDevice->BeginFrame();
    for (int eyeIndex = 0; eyeIndex < 2; eyeIndex++) {
      hmdDevice->BeginRenderingEye(eyeIndex);
      glClearColor(0, 0, 1, 0);
      glClear(GL_COLOR_BUFFER_BIT);
      glFlush();
}

