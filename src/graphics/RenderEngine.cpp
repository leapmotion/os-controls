#include "stdafx.h"
#include "RenderEngine.h"
#include "RenderFrame.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/RenderContext.h"
#include "osinterface/RenderWindow.h"
#include "hmdinterface/IDevice.h"
#include "hmdinterface/IDeviceConfiguration.h"
#include "hmdinterface/IEyeConfiguration.h"

#include <GL/glew.h>
#include "GLShader.h"
#include "GLShaderLoader.h"

#include "Resource.h"
#include "PrimitiveBase.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <thread>
#include <chrono>

RenderEngine::RenderEngine() :
  m_drewFrame(false)
{
  if (!RenderContext::IsShaderAvailable()) {
    throw std::runtime_error("Shaders are not supported!");
  }

  m_shader = Resource<GLShader>("material");

  // set light position
  const EigenTypes::Vector3f lightPos(0, 10, 10);
  m_shader->Bind();
  m_shader->SetUniformf("lightPosition", lightPos);
  m_shader->Unbind();
}

RenderEngine::~RenderEngine()
{
}

void RenderEngine::Tick(std::chrono::duration<double> deltaT) {
  // Active the window for OpenGL rendering
  m_renderWindow->SetActive(true);

  // Clear window
  ::glClearColor(0, 0, 0, 0);
  ::glClear(GL_COLOR_BUFFER_BIT);

  // Set the mode
  ::glEnable(GL_BLEND);
  ::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);

  const auto windowSize = m_renderWindow->GetSize();
  ::glScissor(0, 0, windowSize.width, windowSize.height);
  ::glViewport(0, 0, windowSize.width, windowSize.height);
  m_renderState.GetProjection().Orthographic(0, windowSize.height, windowSize.width, 0, 1, -100);
  m_renderState.GetModelView().Clear();

  m_shader->Bind();

  // Have objects rendering into the specified window with the supplied change in time
  RenderFrame frame{m_renderWindow, m_renderState, deltaT, 0};

  // AnimationUpdate all attached nodes
  for(const auto& renderable : *this)
    renderable->AnimationUpdate(frame);

  bool drewThisFrame = false;

  // Only render objects when the screensaver is disabled
  if (m_virtualScreen && !m_virtualScreen->IsScreenSaverActive()) {

    // Perform render operation in a second pass:
    auto renderFunction = [this, &drewThisFrame, &frame](){
      for (auto& renderable : *this) {
        if (!renderable->IsVisible()) {
          continue;
        }
        drewThisFrame = true;
        auto& mv = frame.renderState.GetModelView();
        mv.Push();

        mv.Translate(EigenTypes::Vector3{ renderable->position.x, renderable->position.y, 0.0 });
        renderable->Render(frame);
        mv.Pop();
      }
    };

    AutowiredFast<Hmd::IDevice> hmd;
    if (hmd) {
      hmd->BeginFrame();
      const EigenTypes::Matrix4x4 projection = frame.renderState.GetProjection().Matrix();

      for (int i = 0; i < hmd->Configuration().EyeCount(); i++) {
        //const int eyeIndex = hmd->Configuration().EyeRenderOrder(i);
        const int eyeIndex = i;
        frame.eyeIndex = eyeIndex;
        hmd->BeginRenderingEye(eyeIndex);
        
        renderFunction();
         
        hmd->EndRenderingEye(eyeIndex);
      }
      hmd->EndFrame();
    }
    else
      renderFunction();
    
  }

  // General cleanup
  m_shader->Unbind();

  // Update the window
  m_renderWindow->FlushBuffer(); // Always update the display, even if we are just erasing
  m_drewFrame = drewThisFrame;

  m_renderWindow->SetActive(false);

  // Show the overlay window if we are rendering, hide it if we aren't
  if (m_renderWindow->IsVisible() != m_drewFrame) {
    if (m_drewFrame) {
      m_renderWindow->SetTransparent(true);
    }
    m_renderWindow->SetVisible(m_drewFrame);
  }
  if (!m_drewFrame) {
    // if we haven't drawn anything, sleep for a bit (otherwise this loop occurs too quickly)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
