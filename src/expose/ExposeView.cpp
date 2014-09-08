#include "stdafx.h"
#include "ExposeView.h"
#include "ExposeViewEvents.h"
#include "ExposeViewWindow.h"
#include "graphics/RenderEngine.h"
#include "graphics/RenderFrame.h"
#include "utility/SamplePrimitives.h"
#include <SVGPrimitive.h>
#include "OSInterface/OSVirtualScreen.h"

ExposeView::ExposeView() :
  m_opacity(0.0f, 0.3f, EasingFunctions::Linear<float>)
{
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

  // Do nothing else if we're invisible
  if(!IsVisible())
    return;

  updateLayout(frame.deltaT);

  for(const auto& renderable : m_zorder)
    renderable->AnimationUpdate(frame);
}

void ExposeView::Render(const RenderFrame& frame) const {
  if(!IsVisible())
    return;

  for(const auto& renderable : m_zorder)
    renderable->Render(frame);
}

void ExposeView::updateLayout(std::chrono::duration<double> dt) {
  // Handle anything pended to the render thread:
  DispatchAllEvents();
  
  double angle = 0;
  const double angleInc = 2*M_PI / static_cast<double>(m_windows.size());

  Autowired<OSVirtualScreen> fullScreen;
  auto screen = fullScreen->PrimaryScreen();

  const Vector2 size(screen.Size().width, screen.Size().height);

  const OSRect bounds = screen.Bounds();
  const Vector2 origin(bounds.origin.x, bounds.origin.y);
  const Vector2 center = origin + 0.5*size;

  const double sizeDiag = size.norm();
  const double radiusPixels = size.norm() * 0.1;

  for(const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if(window->m_layoutLocked)
      continue;

    std::shared_ptr<ImagePrimitive>& img = window->GetTexture();
    const double imgWidth = img->Size().x();
    const double imgHeight = img->Size().y();
    
    const double scale = 0.03;

    const Vector2 point = radialCoordsToPoint(angle, radiusPixels) + center;

    img->Translation().setZero();
    img->Translation().head<2>() = point;
    img->LinearTransformation() = scale * Matrix3x3::Identity();

    angle += angleInc;
  }
}

void ExposeView::focusWindow(ExposeViewWindow& window) {
  // TODO:  Perform the requested action:

  // Operation complete, raise the event:
  m_exposeViewEvents(&ExposeViewEvents::onWindowSelected)(window);
}


Vector2 ExposeView::radialCoordsToPoint(double angle, double distance) {
  return Vector2(distance * std::cos(angle), distance * std::sin(angle));
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
  wnd->RemoveFromParent();
}

void ExposeView::StartView() {
  m_opacity.Set(1.0f, 0.3);
}

void ExposeView::CloseView() {
  m_opacity.Set(0.0f, 0.2f);
}
