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

  // calculate center of the primary screen
  Autowired<OSVirtualScreen> fullScreen;
  auto screen = fullScreen->PrimaryScreen();
  const Vector2 size(screen.Size().width, screen.Size().height);
  const OSRect bounds = screen.Bounds();
  const Vector2 origin(bounds.origin.x, bounds.origin.y);
  const Vector2 center = origin + 0.5*size;

  // calculate radius of layout
  const double radiusPixels = 0.4 * std::min(size.x(), size.y());

  // calculate size of each window
  const double radiusPerWindow = 0.9 * radiusPixels * std::sin(angleInc/2.0);

  for(const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if(window->m_layoutLocked)
      continue;

    std::shared_ptr<ImagePrimitive>& img = window->GetTexture();

    // set window scale smoothly
    const double imgRadius = 0.5 * img->Size().norm();
    const double scale = radiusPerWindow / imgRadius;
    window->m_scale.SetGoal(static_cast<float>(scale));
    window->m_scale.Update(dt.count());
    img->LinearTransformation() = window->m_scale.Value() * Matrix3x3::Identity();

    // calculate position of this window in cartesian coords
    const Vector2 point = radialCoordsToPoint(angle, radiusPixels) + center;

    // set window position smoothly
    const Vector3 point3D(point.x(), point.y(), 0.0);
    window->m_position.SetGoal(point3D);
    window->m_position.Update(dt.count());
    img->Translation() = window->m_position.Value();

    // set window opacity smoothly
    window->m_opacity.SetGoal(1.0f);
    window->m_opacity.Update(dt.count());
    img->SetOpacity(window->m_opacity.Value());

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

  retVal->m_opacity.SetGoal(0.0f);
  retVal->m_opacity.Update(0.0f);
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
