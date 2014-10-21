// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "hmdinterface/OculusRift/RiftContext.h"
#include "hmdinterface/OculusRift/RiftDevice.h"
#include "SDLController.h"

typedef double TimePoint; // TODO: change to std::chrono::time_point once C++11 is fully used.
typedef double TimeDelta; // TODO: change to std::chrono::duration once C++11 is fully used.

class OculusTest : public testing::Test {};

TEST_F(OculusTest, BasicSquare) {
#if 0
  // std::shared_ptr<RenderWindow> window1(RenderWindow::New());

  // std::shared_ptr<OculusRift::Context> hmdContext;
  // std::shared_ptr<OculusRift::Device> hmdDevice;

  // hmdContext = std::make_shared<OculusRift::Context>();
  // hmdDevice = std::make_shared<OculusRift::Device>();
  // hmdContext->Initialize();
  // hmdDevice->SetWindow(window1->GetSystemHandle());
  // hmdDevice->Initialize(*hmdContext);

  // const auto pos = hmdDevice->m_hmd->WindowsPos;
  // const auto size = hmdDevice->m_hmd->Resolution;
#if __APPLE__
  // Apple doesn't have a constructor for CGRect that takes the position and size, so do this manually
  OSRect r;
  // r.origin.x = pos.x + 40;
  // r.origin.y = pos.y + 40;
  // r.size.width = size.w;
  // r.size.height = size.h;
  r.origin.x = 40;
  r.origin.y = 40;
  r.size.width = 640;
  r.size.height = 480;
  window1->SetRect(r);
#else
  // window1->SetRect(OSRect(pos.x, pos.y, size.w, size.h));
  window1->SetRect(OSRect(40, 40, 640, 480));
#endif

  // window1->SetTransparent(true);
  window1->SetVisible(true);
#endif

  SDLController sdl_controller;
  {
    SDLControllerParams params;
    // params.fullscreen = true;
    // params.
    sdl_controller.Initialize(params);
    // sdl_controller.ToggleFullscreen();
  }

  std::shared_ptr<OculusRift::Context> hmdContext;
  std::shared_ptr<OculusRift::Device> hmdDevice;

  hmdContext = std::make_shared<OculusRift::Context>();
  hmdContext->Initialize();

  hmdDevice = std::make_shared<OculusRift::Device>();
  {
    SDL_SysWMinfo sys_wm_info;
    sdl_controller.GetWindowWMInfo(sys_wm_info);

  #if _WIN32
    auto window = sys_wm_info.info.win.window;
  #elif __APPLE__
    auto window = sys_wm_info.info.cocoa.window;
  #else
    auto window = sys_wm_info.info.x11.window;
  #endif

    hmdDevice->SetWindow(window);
    hmdDevice->Initialize(*hmdContext);
  }

  sdl_controller.ResizeWindow(hmdDevice->m_hmd->Resolution.w, hmdDevice->m_hmd->Resolution.h);

  // ovrHmd_DismissHSWDisplay(hmdDevice->m_hmd);

  TimePoint previous_real_time(0.001 * SDL_GetTicks());
  do {
    // TODO: compute the realtime using std::chrono::time_point and time deltas using std::chrono::duration
    TimePoint current_real_time(0.001 * SDL_GetTicks());
    TimeDelta real_time_delta(current_real_time - previous_real_time);

    sdl_controller.BeginRender();
    hmdDevice->BeginFrame();
    for (uint32_t eye_index = 0; eye_index < ovrEye_Count; ++eye_index) {
      hmdDevice->BeginRenderingEye(eye_index);

      // render dummy geometry just as a test
      glEnableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      // glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
      glDisable(GL_LIGHTING);
      glEnable(GL_CULL_FACE);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_DEPTH_TEST);

      static const GLuint VERTEX_COUNT = 4;
      const GLfloat param = 0.1f * std::sin(2.0f*current_real_time) + 0.9f;
      const GLfloat vertex_array[VERTEX_COUNT*2] = {
        -param, -param,
         param, -param,
         param,  param,
        -param,  param
      };

      glColor3f(1.0f, 0.0f, 0.0f);
      glVertexPointer(2, GL_FLOAT, 0, vertex_array);
      glDrawArrays(GL_TRIANGLE_FAN, 0, VERTEX_COUNT);

      hmdDevice->EndRenderingEye(eye_index);
    }
    sdl_controller.EndRender();
    hmdDevice->EndFrame();

    // Save off the updated time for the next loop iteration.
    previous_real_time = current_real_time;
  } while (!SDL_QuitRequested());

  hmdDevice->Shutdown();
  hmdContext->Shutdown();
  // sdl_controller.ToggleFullscreen();
  sdl_controller.Shutdown();

  #if 0
  while (true) {
    
    window1->ProcessEvents();
    // hmdDevice->BeginFrame();
    // for (int eyeIndex = 0; eyeIndex < 2; eyeIndex++) {
      // hmdDevice->BeginRenderingEye(eyeIndex);
      window1->SetActive(true);

      // ::glClearColor(0, 0, 0, 0);
      // ::glClear(GL_COLOR_BUFFER_BIT);

      // const float ar = static_cast<float>(size.w) / static_cast<float>(size.h);
      const float ar = static_cast<float>(640) / static_cast<float>(480);
      // ::glViewport(0, 0, size.w, size.h);
      // ::glMatrixMode(GL_PROJECTION);
      // ::glLoadIdentity();
      // ::glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

      // ::glEnable(GL_BLEND);
      // ::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);

      // ::glPushMatrix();
      // ::glRotatef(rotation, 0.0f, 0.0f, 1.0f);
      // ::glBegin(GL_QUADS);
      // ::glColor3d(1, 0, 0);
      // ::glVertex3f(-1, -1, -5);
      // ::glColor3d(1, 1, 0);
      // ::glVertex3f(1, -1, -5);
      // ::glColor3d(0, 1, 0);
      // ::glVertex3f(1, 1, -5);
      // ::glColor3d(0, 0, 1);
      // ::glVertex3f(-1, 1, -5);
      // ::glEnd();
      // ::glPopMatrix();

      rotation += 0.02f;
      // hmdDevice->EndRenderingEye(eyeIndex);
      // window1->SetActive(false);
    // }
    // hmdDevice->EndFrame();
    window1->FlushBuffer();
    

    // static bool damnit = [hmdDevice]() {
    //   ovrHmd_DismissHSWDisplay(hmdDevice->m_hmd);
    //   return true;
    // }();
  }
  #endif
}

