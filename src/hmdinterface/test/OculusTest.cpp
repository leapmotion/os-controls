// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "hmdinterface/HmdFactory.h"
#include "hmdinterface/IDevice.h"
#include "hmdinterface/IDeviceConfiguration.h"
#include "hmdinterface/IEyeConfiguration.h"
#include "osinterface/RenderWindow.h"
#include "SDLController.h"

#include <iostream>

template <typename Component_, std::size_t SIZE_>
std::ostream &operator << (std::ostream &out, const Hmd::IntermediateArray<Component_,SIZE_> &array) {
  out << '(';
  static_assert(SIZE_ > 0, "Can't have a zero-sized IntermediateArray.");
  for (size_t i = 0; i < SIZE_-1; ++i) {
    out << array[i] << ',';
  }
  return out << array[SIZE_-1] << ')';
}


typedef double TimePoint; // TODO: change to std::chrono::time_point once C++11 is fully used.
typedef double TimeDelta; // TODO: change to std::chrono::duration once C++11 is fully used.

class OculusTest : public testing::Test {};

TEST_F(OculusTest, BasicSquare) {
  Hmd::HmdFactory factory;
  
  std::shared_ptr<Hmd::IDevice> hmdDevice(factory.CreateDevice());
  
  SDLController sdl_controller;
  {
    SDLControllerParams params;
    params.windowFlags |= SDL_WINDOW_BORDERLESS;
    sdl_controller.Initialize(params);

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
    hmdDevice->Initialize();

    auto &cfg = hmdDevice->Configuration();
    sdl_controller.ResizeWindow(cfg.DisplayWidth(), cfg.DisplayHeight());
    // For whatever reason, this seems to be necessary to get the Oculus stuff to render to the resized window.
    sdl_controller.BeginRender();
    sdl_controller.EndRender();
    // Setting the window position is what puts the window into the correct position in an extended display.
    std::cerr << "window pos : " << cfg.WindowPositionX() << ", " << cfg.WindowPositionY() << " (note: this may put the window off the primary display, onto the Oculus Rift's display)\n";
    sdl_controller.RepositionWindow(cfg.WindowPositionX(), cfg.WindowPositionY());
  }

  // There is some fanciness in OVR land about not being able to dismiss until after some timeout.
  // TODO: figure out how to dismiss it unconditionally.
  hmdDevice->DismissHealthWarning();

  TimePoint previous_real_time(0.001 * SDL_GetTicks());
  do {
    // TODO: compute the realtime using std::chrono::time_point and time deltas using std::chrono::duration
    TimePoint current_real_time(0.001 * SDL_GetTicks());
    TimeDelta real_time_delta(current_real_time - previous_real_time);

    // sdl_controller.BeginRender();
    hmdDevice->BeginFrame();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    for (uint32_t eye_render_index = 0; eye_render_index < hmdDevice->Configuration().EyeCount(); ++eye_render_index) {
      uint32_t eye_index = hmdDevice->Configuration().EyeRenderOrder(eye_render_index);
      hmdDevice->BeginRenderingEye(eye_index);

      auto eye_pose = hmdDevice->EyePose(eye_index);
      auto &eye_configuration = hmdDevice->Configuration().EyeConfiguration(eye_index);

      glDisable(GL_LIGHTING);
      glDisable(GL_CULL_FACE);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_DEPTH_TEST);
      // glEnable(GL_BLEND);
      // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // Set up the projection matrix for this eye.
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixd(eye_configuration.ProjectionMatrix(0.1, 10000.0, Hmd::MatrixComponentOrder::COLUMN_MAJOR).data());

      // Set up the modelview matrix -- use the view matrix for this eye.
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixd(eye_pose->ViewMatrix(Hmd::MatrixComponentOrder::COLUMN_MAJOR, eye_configuration).data());

      // Render a spherical grid centered at the viewpoint
      {
        const int divTheta = 22;
        const int divPhi = 40;
        const float radius = 10.0f;

        Hmd::DoubleArray<3> eye_position(eye_pose->Position());
        // std::cout << "eye " << eye_index << " position: " << eye_position << '\n';        
        glTranslated(eye_position[0], eye_position[1], eye_position[2]);
        glColor4f(0.2f, 0.6f, 1.0f, 0.5f);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        for (int i = 0; i < divPhi; i++) {
          float phi0 = (float)M_PI*(i/static_cast<float>(divPhi) - 0.5f);
          float phi1 = (float)M_PI*((i + 1)/static_cast<float>(divPhi) - 0.5f);
          for (int j = 0; j < divTheta; j++) {
            float theta0 = 2*(float)M_PI*(j/static_cast<float>(divTheta));
            float theta1 = 2*(float)M_PI*((j + 1)/static_cast<float>(divTheta));
            glVertex3f(radius*cos(phi0)*cos(theta0), radius*sin(phi0), radius*cos(phi0)*sin(theta0));
            glVertex3f(radius*cos(phi0)*cos(theta1), radius*sin(phi0), radius*cos(phi0)*sin(theta1));
            glVertex3f(radius*cos(phi0)*cos(theta0), radius*sin(phi0), radius*cos(phi0)*sin(theta0));
            glVertex3f(radius*cos(phi1)*cos(theta0), radius*sin(phi1), radius*cos(phi1)*sin(theta0));
          }
        }
        glEnd();        
      }

      hmdDevice->EndRenderingEye(eye_index);
    }
    // sdl_controller.EndRender();
    hmdDevice->EndFrame();

    // Save off the updated time for the next loop iteration.
    previous_real_time = current_real_time;
  } while (!SDL_QuitRequested());

  hmdDevice->Shutdown();
  // sdl_controller.ToggleFullscreen();
  sdl_controller.Shutdown();
}


TEST_F(OculusTest, BasicSquareRenderWindow) {
  Hmd::HmdFactory factory;
  std::shared_ptr<Hmd::IDevice> hmdDevice(factory.CreateDevice());

  std::unique_ptr<RenderWindow> renderWindow = std::unique_ptr<RenderWindow>(RenderWindow::New());
  renderWindow->SetActive(true);

  hmdDevice->SetWindow(renderWindow->GetSystemHandle());
  hmdDevice->Initialize();
  auto &cfg = hmdDevice->Configuration();

  renderWindow->SetSize(OSSize{CGFloat(cfg.DisplayWidth()), CGFloat(cfg.DisplayHeight())});

  renderWindow->SetPosition(OSPoint{CGFloat(cfg.WindowPositionX()), CGFloat(cfg.WindowPositionY())});
  renderWindow->ProcessEvents();
  
  // There is some fanciness in OVR land about not being able to dismiss until after some timeout.
  // TODO: figure out how to dismiss it unconditionally.
  hmdDevice->DismissHealthWarning();

  TimePoint previous_real_time(0.001 * SDL_GetTicks());
  while (true) {
    renderWindow->ProcessEvents();
    // TODO: compute the realtime using std::chrono::time_point and time deltas using std::chrono::duration
    TimePoint current_real_time(0.001 * SDL_GetTicks());
    TimeDelta real_time_delta(current_real_time - previous_real_time);

    // sdl_controller.BeginRender();
    hmdDevice->BeginFrame();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (uint32_t eye_render_index = 0; eye_render_index < hmdDevice->Configuration().EyeCount(); ++eye_render_index) {
      uint32_t eye_index = hmdDevice->Configuration().EyeRenderOrder(eye_render_index);
      hmdDevice->BeginRenderingEye(eye_index);

      auto eye_pose = hmdDevice->EyePose(eye_index);
      auto &eye_configuration = hmdDevice->Configuration().EyeConfiguration(eye_index);

      glDisable(GL_LIGHTING);
      glDisable(GL_CULL_FACE);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_DEPTH_TEST);

      // Set up the projection matrix for this eye.
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixd(eye_configuration.ProjectionMatrix(0.1, 10000.0, Hmd::MatrixComponentOrder::COLUMN_MAJOR).data());

      // Set up the modelview matrix -- use the view matrix for this eye.
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixd(eye_pose->ViewMatrix(Hmd::MatrixComponentOrder::COLUMN_MAJOR, eye_configuration).data());

      // Render a spherical grid centered at the viewpoint
      {
        const int divTheta = 22;
        const int divPhi = 40;
        const float radius = 10.0f;

        Hmd::DoubleArray<3> eye_position(eye_pose->Position());
        // std::cout << "eye " << eye_index << " position: " << eye_position << '\n';        
        glTranslated(eye_position[0], eye_position[1], eye_position[2]);
        glColor4f(0.2f, 0.6f, 1.0f, 0.5f);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        for (int i = 0; i < divPhi; i++) {
          float phi0 = (float)M_PI*(i / static_cast<float>(divPhi)-0.5f);
          float phi1 = (float)M_PI*((i + 1) / static_cast<float>(divPhi)-0.5f);
          for (int j = 0; j < divTheta; j++) {
            float theta0 = 2 * (float)M_PI*(j / static_cast<float>(divTheta));
            float theta1 = 2 * (float)M_PI*((j + 1) / static_cast<float>(divTheta));
            glVertex3f(radius*cos(phi0)*cos(theta0), radius*sin(phi0), radius*cos(phi0)*sin(theta0));
            glVertex3f(radius*cos(phi0)*cos(theta1), radius*sin(phi0), radius*cos(phi0)*sin(theta1));
            glVertex3f(radius*cos(phi0)*cos(theta0), radius*sin(phi0), radius*cos(phi0)*sin(theta0));
            glVertex3f(radius*cos(phi1)*cos(theta0), radius*sin(phi1), radius*cos(phi1)*sin(theta0));
          }
        }
        glEnd();
      }

      hmdDevice->EndRenderingEye(eye_index);
    }
    // sdl_controller.EndRender();

    hmdDevice->EndFrame();
    // Save off the updated time for the next loop iteration.
    previous_real_time = current_real_time;
  }

  hmdDevice->Shutdown();
}

