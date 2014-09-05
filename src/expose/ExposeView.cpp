#include "stdafx.h"
#include "ExposeView.h"
#include "ExposeViewEvents.h"
#include "ExposeViewWindow.h"
#include "graphics/RenderEngine.h"
#include "graphics/RenderFrame.h"
#include "utility/SamplePrimitives.h"
#include <SVGPrimitive.h>

ExposeView::ExposeView() :
  m_opacity(0.0f, 0.3f, EasingFunctions::Linear<float>)
{
  // Add a box as our child
  m_box = MakeBoxPrimitive();
}

ExposeView::~ExposeView() {
  
}

void ExposeView::AutoInit() {
  m_rootNode.NotifyWhenAutowired([this]{
    m_rootNode->Add(shared_from_this());
  });
}

void ExposeView::AnimationUpdate(const RenderFrame& frame) {
  m_opacity.Update(frame.deltaT.count());
  updateLayout(frame.deltaT);

  for(const auto& renderable : m_zorder)
    renderable->AnimationUpdate(frame);
}

void ExposeView::Render(const RenderFrame& frame) const {
  for(const auto& renderable : m_zorder)
    renderable->Render(frame);
  m_box->DrawSceneGraph(*m_box, frame.renderState);
}

void ExposeView::updateLayout(std::chrono::duration<double> dt) {
  // Handle anything pended to the render thread:
  DispatchAllEvents();
  
  for(const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if(window->m_layoutLocked)
      continue;

    // TODO:  Update the position of the current window
    
  }
}

void ExposeView::focusWindow(ExposeViewWindow& window) {
  // TODO:  Perform the requested action:

  // Operation complete, raise the event:
  m_exposeViewEvents(&ExposeViewEvents::onWindowSelected)(window);
}


std::tuple<double, double> ExposeView::radialCoordsToPoint(double angle, double distance) {
  return std::make_tuple(0.0, 0.0);
}

std::shared_ptr<ExposeViewWindow> ExposeView::NewExposeWindow(OSWindow& osWindow) {
  auto retVal = std::shared_ptr<ExposeViewWindow>(new ExposeViewWindow(osWindow));
  m_windows.insert(retVal);
  m_zorder.Add(retVal);

  // Update the window texture in the main render loop:
  *this += [retVal] {
    retVal->UpdateTexture();
  };
  return retVal;
}

void ExposeView::RemoveExposeWindow(const std::shared_ptr<ExposeViewWindow>& wnd) {
  m_windows.erase(wnd);
}

void ExposeView::StartView() {
  m_opacity.Set(1.0f, 0.3);
}

void ExposeView::CloseView() {
  m_opacity.Set(0.0f, 0.2f);
}
