#include "stdafx.h"
#include "ExposeView.h"
#include "ExposeViewEvents.h"
#include "ExposeViewWindow.h"
#include "graphics/RenderEngine.h"
#include "graphics/RenderFrame.h"
#include "utility/SamplePrimitives.h"
#include <SVGPrimitive.h>
#include "OSInterface/OSVirtualScreen.h"
#include "OSInterface/OSWindow.h"

Color selectionRegionColor(1.0f, 1.0f, 1.0f, 0.15f);
Color selectionOutlineColor(1.0f, 1.0f, 1.0f, 0.3f);
Color selectionRegionActiveColor(0.5f, 1.0f, 0.7f, 0.25f);
Color selectionOutlineActiveColor(0.5f, 1.0f, 0.7f, 0.5f);

ExposeView::ExposeView() :
  m_alphaMask(0.0f, 0.3f, EasingFunctions::Linear<float>),
  m_layoutRadius(500.0),
  m_selectionRadius(100),
  m_viewCenter(Vector2::Zero())
{

  m_backgroundRect = std::shared_ptr<RectanglePrim>(new RectanglePrim);

  m_selectionRegion = std::shared_ptr<Disk>(new Disk);
  m_selectionRegion->Material().SetDiffuseLightColor(selectionRegionColor);
  m_selectionRegion->Material().SetAmbientLightColor(selectionRegionColor);
  m_selectionRegion->Material().SetAmbientLightingProportion(1.0f);

  m_selectionOutline = std::shared_ptr<PartialDisk>(new PartialDisk);
  m_selectionOutline->Material().SetDiffuseLightColor(selectionOutlineColor);
  m_selectionOutline->Material().SetAmbientLightColor(selectionOutlineColor);
  m_selectionOutline->Material().SetAmbientLightingProportion(1.0f);

  Color backgroundRectColor(0.05f, 0.05f, 0.05f, 0.9f);
  m_backgroundRect->Material().SetDiffuseLightColor(backgroundRectColor);
  m_backgroundRect->Material().SetAmbientLightColor(backgroundRectColor);
  m_backgroundRect->Material().SetAmbientLightingProportion(1.0f);
}

ExposeView::~ExposeView() {
  
}

void ExposeView::AutoInit() {
  m_rootNode.NotifyWhenAutowired([this]{
    m_rootNode->Add(std::static_pointer_cast<ExposeView>(shared_from_this()));
  });
}

void ExposeView::AnimationUpdate(const RenderFrame& frame) {
  m_alphaMask.Update(frame.deltaT.count());

  // Do nothing else if we're invisible
  if(!IsVisible())
    return;

  updateLayout(frame.deltaT);
  updateActivations(frame.deltaT);
  updateForces(frame.deltaT);

  for(const auto& renderable : m_zorder)
    renderable->AnimationUpdate(frame);
}

void ExposeView::Render(const RenderFrame& frame) const {
  if(!IsVisible())
    return;

  PrimitiveBase::DrawSceneGraph(*m_backgroundRect, frame.renderState);

  PrimitiveBase::DrawSceneGraph(*m_selectionRegion, frame.renderState);
  PrimitiveBase::DrawSceneGraph(*m_selectionOutline, frame.renderState);

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
  m_viewCenter = origin + 0.5*size;

  // update background rectangle
  m_backgroundRect->SetSize(size - Vector2(0, 1));
  m_backgroundRect->Translation() << m_viewCenter.x(), m_viewCenter.y(), 0.0;

  // calculate radius of layout
  m_layoutRadius = 0.4 * std::min(size.x(), size.y());
  m_selectionRadius = 0.5 * m_layoutRadius;

  // calculate size of each window
  const double radiusPerWindow = 0.9* m_layoutRadius * std::sin(angleInc/2.0);

  for(const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if(window->m_layoutLocked)
      continue;

    std::shared_ptr<ImagePrimitive>& img = window->GetTexture();

    // set window scale smoothly
    const double imgRadius = 0.5 * img->Size().norm();
    const double bonusScale = 0.5 * (window->m_hover.Value() + window->m_activation.Value());
    const double scale = (1.0 + bonusScale) * radiusPerWindow / imgRadius;
    window->m_scale.SetGoal(static_cast<float>(scale));
    window->m_scale.Update(dt.count());
    img->LinearTransformation() = window->m_scale.Value() * Matrix3x3::Identity();

    // calculate position of this window in cartesian coords
    const Vector2 point = radialCoordsToPoint(angle, m_layoutRadius) + m_viewCenter;
    const Vector3 point3D(point.x(), point.y(), 0.0);

    Vector3 totalForce(Vector3::Zero());
    for (size_t i=0; i<m_forces.size(); i++) {
      if (m_forces[i].m_window != window.get()) {
        totalForce += m_forces[i].ForceAt(point3D);
      }
    }

    // set window position smoothly
    window->m_position.SetGoal(point3D + totalForce);
    window->m_position.Update(dt.count());
    img->Translation() = window->m_position.Value() + window->m_grabDelta.Value();

    // set window opacity smoothly
    window->m_opacity.SetGoal(1.0f);
    window->m_opacity.Update(dt.count());
    img->LocalProperties().AlphaMask() = window->m_opacity.Value() * m_alphaMask.Current();

    angle += angleInc;
  }

  m_selectionOutline->Translation() << m_viewCenter.x(), m_viewCenter.y(), 0.0;
  m_selectionOutline->SetInnerRadius(m_selectionRadius);
  m_selectionOutline->SetOuterRadius(1.005*m_selectionRadius);
  m_selectionOutline->LocalProperties().AlphaMask() = m_alphaMask.Current();

  m_selectionRegion->Translation() << m_viewCenter.x(), m_viewCenter.y(), 0.0;
  m_selectionRegion->SetRadius(m_selectionRadius);
  m_selectionRegion->LocalProperties().AlphaMask() = m_alphaMask.Current();

  m_backgroundRect->LocalProperties().AlphaMask() = m_alphaMask.Current();
}

void ExposeView::updateActivations(std::chrono::duration<double> dt) {
#if 0
  POINT p;
  GetCursorPos(&p);
  const Vector2 handPos(p.x, p.y);
  const float activation = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1.0f : 0.0f;
#else
  const Vector2& handPos = m_handData.locationData.screenPosition();
  const float activation = std::max(m_handData.grabData.grabStrength, m_handData.pinchData.pinchStrength);
#endif
  static Vector2 prevHandPos = handPos;

  ExposeViewWindow* closestWindow = nullptr;
  double closestDistSq = DBL_MAX;
  const double distSqThreshPixels = 100;

  for (const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if (window->m_layoutLocked)
      continue;

    const std::shared_ptr<ImagePrimitive>& img = window->GetTexture();
    
    const Vector2 windowPos = img->Translation().head<2>();
    const float scale = window->m_scale.Value();

    const Vector2 windowSize = scale * img->Size();

    const Vector2 min = windowPos - 0.5*windowSize;
    const Vector2 max = windowPos + 0.5*windowSize;

    const Vector2 closestPoint = handPos.cwiseMin(max).cwiseMax(min);

    const double distSq = (handPos - closestPoint).squaredNorm();
    const double modifiedDistSq = (1.0f - window->m_activation.Value()) * (distSq + 0.9999*distSqThreshPixels);
    if (modifiedDistSq < closestDistSq) {
      closestWindow = window.get();
      closestDistSq = modifiedDistSq;
    }
  }

  float maxSelection = 0;

  for (const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if (window->m_layoutLocked)
      continue;

    const std::shared_ptr<ImagePrimitive>& img = window->GetTexture();

    if (window.get() == closestWindow && closestDistSq < distSqThreshPixels && !window->m_cooldown) {
      window->m_hover.SetGoal(1.0f);
      window->m_activation.SetGoal(activation * window->m_hover.Value());
      Vector3 displacement = Vector3::Zero();
      displacement.head<2>() = handPos - prevHandPos;
      window->m_grabDelta.SetSmoothStrength(0.25f);
      window->m_grabDelta.SetGoal(activation*(window->m_grabDelta.Goal() + displacement));

      if ((img->Translation() - Vector3(m_viewCenter.x(), m_viewCenter.y(), 0.0)).squaredNorm() < m_selectionRadius*m_selectionRadius) {
        window->m_selection.SetGoal(activation * window->m_activation.Value());

        if (activation < window->m_selection.Value()) {
          focusWindow(*window);
          window->m_cooldown = true;          
        }

      } else {
        window->m_selection.SetGoal(0.0f);
      }
    } else {
      window->m_hover.SetGoal(0.0f);
      window->m_activation.SetGoal(0.0f);
      window->m_grabDelta.SetSmoothStrength(0.85f);
      window->m_grabDelta.SetGoal(Vector3::Zero());
      window->m_selection.SetGoal(0.0f);
    }
    window->m_hover.Update(dt.count());
    window->m_activation.Update(dt.count());
    window->m_grabDelta.Update(dt.count());
    window->m_selection.Update(dt.count());

    if (window->m_cooldown && window->m_hover.Value() < 0.1f) {
      window->m_cooldown = false;
    }

    maxSelection = std::max(maxSelection, window->m_selection.Value());
  }

  const Vector4f regionColor = maxSelection*selectionRegionActiveColor.Data() + (1.0f - maxSelection)*selectionRegionColor.Data();
  const Vector4f outlineColor = maxSelection*selectionOutlineActiveColor.Data() + (1.0f - maxSelection)*selectionOutlineColor.Data();
  
  m_selectionRegion->Material().SetDiffuseLightColor(regionColor);
  m_selectionRegion->Material().SetAmbientLightColor(regionColor);
  m_selectionOutline->Material().SetDiffuseLightColor(outlineColor);
  m_selectionOutline->Material().SetAmbientLightColor(outlineColor);

  prevHandPos = handPos;
}

void ExposeView::updateForces(std::chrono::duration<double> dt) {
  m_forces.clear();
  static const double MAX_RADIUS_MULT = 1.0;
  static const double FORCE_DISTANCE_MULT = 0.1;
  for (const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if (window->m_layoutLocked)
      continue;

    std::shared_ptr<ImagePrimitive>& img = window->GetTexture();
    if (window->m_hover.Value() > 0.0001f) {
      m_forces.push_back(Force(img->Translation(), FORCE_DISTANCE_MULT*m_layoutRadius*(window->m_hover.Value() + window->m_activation.Value()), window.get(), MAX_RADIUS_MULT*m_layoutRadius));
    }
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

  retVal->m_scale.SetGoal(0.0f);
  retVal->m_scale.Update(0.0f);

  retVal->m_activation.SetGoal(0.0f);
  retVal->m_activation.Update(0.0f);

  retVal->m_hover.SetGoal(0.0f);
  retVal->m_hover.Update(0.0f);

  retVal->m_selection.SetGoal(0.0f);
  retVal->m_selection.Update(0.0f);

  retVal->m_grabDelta.SetGoal(Vector3::Zero());
  retVal->m_grabDelta.Update(0.0f);

  return retVal;
}

void ExposeView::RemoveExposeWindow(const std::shared_ptr<ExposeViewWindow>& wnd) {
  m_windows.erase(wnd);
  wnd->RemoveFromParent();
}

void ExposeView::StartView() {
  m_alphaMask.Set(1.0f, 0.3);
}

void ExposeView::CloseView() {
  m_alphaMask.Set(0.0f, 0.2f);
}
