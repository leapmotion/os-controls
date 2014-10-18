// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "hmdinterface/OculusRift/RiftContext.h"
#include "hmdinterface/OculusRift/RiftDevice.h"
#include "osinterface/RenderWindow.h"

class OculusTest :
public testing::Test
{};

TEST_F(OculusTest, BasicSquare) {
  glewInit();

  std::shared_ptr<RenderWindow> window1(RenderWindow::New());

  auto hmdContext = std::make_shared<OculusRift::Context>();
  auto hmdDevice = std::make_shared<OculusRift::Device>();
  hmdContext->Initialize();
  hmdDevice->SetWindow(window1->GetSystemHandle());
  hmdDevice->Initialize(*hmdContext);

  const auto pos = hmdDevice->m_hmd->WindowsPos;
  const auto size = hmdDevice->m_hmd->Resolution;
  window1->SetRect(OSRect(pos.x, pos.y, size.w, size.h));

  window1->SetTransparent(true);
  window1->SetVisible(true);

  float rotation = 0;
  while (true) {
    
    window1->ProcessEvents();
    hmdDevice->BeginFrame();
    for (int eyeIndex = 0; eyeIndex < 2; eyeIndex++) {
      hmdDevice->BeginRenderingEye(eyeIndex);
      window1->SetActive(true);

      ::glClearColor(0, 0, 0, 0);
      ::glClear(GL_COLOR_BUFFER_BIT);

      const float ar = static_cast<float>(size.w) / static_cast<float>(size.h);
      ::glViewport(0, 0, size.w, size.h);
      ::glMatrixMode(GL_PROJECTION);
      ::glLoadIdentity();
      ::glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

      ::glEnable(GL_BLEND);
      ::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);

      ::glPushMatrix();
      ::glRotatef(rotation, 0.0f, 0.0f, 1.0f);
      ::glBegin(GL_QUADS);
      ::glColor3d(1, 0, 0);
      ::glVertex3f(-1, -1, -5);
      ::glColor3d(1, 1, 0);
      ::glVertex3f(1, -1, -5);
      ::glColor3d(0, 1, 0);
      ::glVertex3f(1, 1, -5);
      ::glColor3d(0, 0, 1);
      ::glVertex3f(-1, 1, -5);
      ::glEnd();
      ::glPopMatrix();

      rotation += 4.0f;
      hmdDevice->EndRenderingEye(eyeIndex);
      window1->SetActive(false);
    }
    hmdDevice->EndFrame();

    static bool damnit = [hmdDevice]() {
      ovrHmd_DismissHSWDisplay(hmdDevice->m_hmd);
      return true;
    }();
  }
}

