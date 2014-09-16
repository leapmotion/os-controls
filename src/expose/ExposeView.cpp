#include "stdafx.h"
#include "ExposeView.h"
#include "ExposeViewEvents.h"
#include "ExposeViewWindow.h"
#include "graphics/RenderEngine.h"
#include "graphics/RenderFrame.h"
#include "utility/NativeWindow.h"
#include "utility/SamplePrimitives.h"
#include <SVGPrimitive.h>
#include "OSInterface/OSApp.h"
#include "OSInterface/OSVirtualScreen.h"
#include "OSInterface/OSWindow.h"

Color selectionRegionColor(1.0f, 1.0f, 1.0f, 0.15f);
Color selectionOutlineColor(1.0f, 1.0f, 1.0f, 0.3f);
Color selectionRegionActiveColor(0.5f, 1.0f, 0.7f, 0.25f);
Color selectionOutlineActiveColor(0.5f, 1.0f, 0.7f, 0.5f);

ExposeView::ExposeView() :
  m_alphaMask(0.0f, 1.0f, EasingFunctions::Linear<float>),
  m_layoutRadius(500.0),
  m_selectionRadius(100),
  m_viewCenter(Vector2::Zero())
{
  m_backgroundImage = std::shared_ptr<ImagePrimitive>(new ImagePrimitive);
  m_backgroundImage = Autowired<OSVirtualScreen>()->PrimaryScreen().GetBackgroundTexture(m_backgroundImage);

  m_selectionRegion = std::shared_ptr<Disk>(new Disk);
  m_selectionRegion->Material().SetDiffuseLightColor(selectionRegionColor);
  m_selectionRegion->Material().SetAmbientLightColor(selectionRegionColor);
  m_selectionRegion->Material().SetAmbientLightingProportion(1.0f);

  m_selectionOutline = std::shared_ptr<PartialDisk>(new PartialDisk);
  m_selectionOutline->Material().SetDiffuseLightColor(selectionOutlineColor);
  m_selectionOutline->Material().SetAmbientLightColor(selectionOutlineColor);
  m_selectionOutline->Material().SetAmbientLightingProportion(1.0f);

  m_selectionRegionActive = std::shared_ptr<Disk>(new Disk);
  m_selectionRegionActive->Material().SetDiffuseLightColor(selectionRegionActiveColor);
  m_selectionRegionActive->Material().SetAmbientLightColor(selectionRegionActiveColor);
  m_selectionRegionActive->Material().SetAmbientLightingProportion(1.0f);

  m_selectionOutlineActive = std::shared_ptr<PartialDisk>(new PartialDisk);
  m_selectionOutlineActive->Material().SetDiffuseLightColor(selectionOutlineActiveColor);
  m_selectionOutlineActive->Material().SetAmbientLightColor(selectionOutlineActiveColor);
  m_selectionOutlineActive->Material().SetAmbientLightingProportion(1.0f);
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
  updateWindowTexturesRoundRobin();

  m_orderedWindows.clear();
  m_orderedWindows.reserve(m_windows.size());
  for (const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    m_orderedWindows.push_back(window);
  }
  std::sort(m_orderedWindows.begin(), m_orderedWindows.end(),
    [] (const std::shared_ptr<ExposeViewWindow>& win1, const std::shared_ptr<ExposeViewWindow>& win2) {
      return win1->m_osWindow->GetZOrder() < win2->m_osWindow->GetZOrder();
    }
  );

  for (const auto& renderable : m_orderedWindows)
    renderable->AnimationUpdate(frame);
}

void ExposeView::Render(const RenderFrame& frame) const {
  if(!IsVisible())
    return;

  PrimitiveBase::DrawSceneGraph(*m_backgroundImage, frame.renderState);

  PrimitiveBase::DrawSceneGraph(*m_selectionRegion, frame.renderState);
  PrimitiveBase::DrawSceneGraph(*m_selectionOutline, frame.renderState);

  for (const std::shared_ptr<ExposeViewWindow>& window : m_orderedWindows) {
    window->Render(frame);
  }

  for (const std::shared_ptr<ExposeGroup>& group : m_groups) {
    group->Render(frame);
  }

  PrimitiveBase::DrawSceneGraph(*m_selectionRegionActive, frame.renderState);
  PrimitiveBase::DrawSceneGraph(*m_selectionOutlineActive, frame.renderState);
}

void ExposeView::updateLayout(std::chrono::duration<double> dt) {
  // Handle anything pended to the render thread:
  DispatchAllEvents();

  // calculate center of the primary screen
  Autowired<OSVirtualScreen> fullScreen;
  const Vector2 fullSize(fullScreen->Size().width, fullScreen->Size().height);
  const OSRect fullBounds = fullScreen->Bounds();
  const Vector2 fullOrigin(fullBounds.origin.x, fullBounds.origin.y);
  const Vector2 fullCenter = fullOrigin + 0.5*fullSize;

  auto screen = fullScreen->PrimaryScreen();
  const Vector2 size(screen.Size().width, screen.Size().height);
  const OSRect bounds = screen.Bounds();
  const Vector2 origin(bounds.origin.x, bounds.origin.y);
  m_viewCenter = origin + 0.5*size;

  // update background rectangle
  const Vector2& bgSize = m_backgroundImage->Size();
  const double bgAspect = bgSize.x() / bgSize.y();
#if _WIN32
  const Vector2 screenSize = fullSize;
  const Vector2 screenCenter = fullCenter;
#else
  const Vector2 screenSize = size;
  const Vector2 screenCenter = m_viewCenter;
#endif
  const double fullAspect = screenSize.x() / screenSize.y();
  m_backgroundImage->Translation() << screenCenter.x(), screenCenter.y(), 0.0;

  double bgScale = 1.0;
  if (bgAspect > fullAspect) {
    bgScale = screenSize.y() / bgSize.y();
  } else {
    bgScale = screenSize.x() / bgSize.x();
  }
  m_backgroundImage->LinearTransformation() = bgScale*Matrix3x3::Identity();

  // calculate radius of layout
  m_layoutRadius = 0.4 * std::min(size.x(), size.y());
  m_selectionRadius = 0.5 * m_layoutRadius;

  const Vector2 screenToFullScale = size.cwiseQuotient(fullSize);
  const double radiusPerWindow = 0.75 * m_layoutRadius * std::sin(std::min(M_PI/2.0, M_PI / static_cast<double>(m_windows.size())));

  for (const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if (window->m_layoutLocked)
      continue;

    std::shared_ptr<ImagePrimitive>& img = window->GetTexture();

    // set window scale smoothly
    const double bonusScale = 0.2 * (window->m_hover.Value() + window->m_activation.Value());
    const double imgRadius = 0.5 * img->Size().norm();
    const double scale = 4.0 * (1.0 + bonusScale) * (radiusPerWindow / imgRadius) * size.norm() / fullSize.norm();
    if (!m_closing) {
      window->m_scale.SetGoal(static_cast<float>(scale));
    }
    window->m_scale.Update((float)dt.count());
    img->LinearTransformation() = window->m_scale.Value() * Matrix3x3::Identity();

    Vector3 totalForce(Vector3::Zero());

    if (!m_closing) {
      for (size_t i=0; i<m_forces.size(); i++) {
        if (m_forces[i].m_window != window.get()) {
          totalForce += m_forces[i].ForceAt(img->Translation());
        }
      }
    }

    window->m_forceDelta.SetGoal(totalForce);
    window->m_position.Update(static_cast<float>(dt.count()));
    window->m_forceDelta.Update(static_cast<float>(dt.count()));
    img->Translation() = window->m_position.Value() + window->m_grabDelta.Value() + window->m_forceDelta.Value();

    // set window opacity smoothly
    window->m_opacity.SetGoal(1.0f);
    window->m_opacity.Update(static_cast<float>(dt.count()));
    img->LocalProperties().AlphaMask() = window->m_opacity.Value() * m_alphaMask.Current();
  }

  m_selectionOutline->Translation() << m_viewCenter.x(), m_viewCenter.y(), 0.0;
  m_selectionOutline->SetInnerRadius(m_selectionRadius);
  m_selectionOutline->SetOuterRadius(1.005*m_selectionRadius);
  m_selectionOutline->LocalProperties().AlphaMask() = m_alphaMask.Current();

  m_selectionRegion->Translation() << m_viewCenter.x(), m_viewCenter.y(), 0.0;
  m_selectionRegion->SetRadius(m_selectionRadius);
  m_selectionRegion->LocalProperties().AlphaMask() = m_alphaMask.Current();

  m_selectionOutlineActive->Translation() << m_viewCenter.x(), m_viewCenter.y(), 0.0;
  m_selectionOutlineActive->SetInnerRadius(m_selectionRadius);
  m_selectionOutlineActive->SetOuterRadius(1.005*m_selectionRadius);

  m_selectionRegionActive->Translation() << m_viewCenter.x(), m_viewCenter.y(), 0.0;
  m_selectionRegionActive->SetRadius(m_selectionRadius);
  m_selectionRegionActive->LocalProperties().AlphaMask() = m_alphaMask.Current();

  m_backgroundImage->LocalProperties().AlphaMask() = m_alphaMask.Current();

  for (const std::shared_ptr<ExposeGroup>& group : m_groups) {
    Vector3 center(Vector3::Zero());
    double scale = 0;
    double weight = 0;
    assert(!group->m_groupMembers.empty());
    for (const std::shared_ptr<ExposeViewWindow>& window : group->m_groupMembers) {
      const double curWeight = window->GetTexture()->Size().norm() * window->m_opacity.Value();
      center += curWeight * window->GetTexture()->Translation();
      weight += curWeight;
      scale += curWeight * window->GetTexture()->LinearTransformation()(0, 0);
    }
    center /= weight;
    scale /= weight;
    group->m_icon->Translation() = center;

    group->m_icon->LinearTransformation() = (1.5 * scale) * Matrix3x3::Identity();
    group->m_icon->LocalProperties().AlphaMask() = m_alphaMask.Current();
  }
}

void ExposeView::startPositions() {
  for (const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if (window->m_layoutLocked)
      continue;

    window->SetOpeningPosition();
  }
}

void ExposeView::endPositions() {
  for (const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if (window->m_layoutLocked)
      continue;

    window->SetClosingPosition();
  }
}

void ExposeView::updateActivations(std::chrono::duration<double> dt) {
#if 0
  POINT p;
  GetCursorPos(&p);
  const Vector2 handPos(p.x, p.y);
  const float activation = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1.0f : 0.0f;
#else
  const Vector2& handPos = m_handData.locationData.screenPosition();
  const float grabPinch = std::max(m_handData.grabData.grabStrength, m_handData.pinchData.pinchStrength);
  const float activation = m_alphaMask.Current() > 0.99f ? grabPinch : 0.0f;
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
      window->m_grabDelta.SetSmoothStrength(0.5f);
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
      window->m_grabDelta.SetSmoothStrength(0.9f);
      window->m_grabDelta.SetGoal(Vector3::Zero());
      window->m_selection.SetGoal(0.0f);
    }
    window->m_hover.Update((float)dt.count());
    window->m_activation.Update((float)dt.count());
    window->m_grabDelta.Update((float)dt.count());
    window->m_selection.Update((float)dt.count());

    if (window->m_cooldown && window->m_hover.Value() < 0.1f) {
      window->m_cooldown = false;
    }

    maxSelection = std::max(maxSelection, window->m_selection.Value());
  }

#if 0
  const Vector4f regionColor = maxSelection*selectionRegionActiveColor.Data() + (1.0f - maxSelection)*selectionRegionColor.Data();
  const Vector4f outlineColor = maxSelection*selectionOutlineActiveColor.Data() + (1.0f - maxSelection)*selectionOutlineColor.Data();
  
  m_selectionRegion->Material().SetDiffuseLightColor(regionColor);
  m_selectionRegion->Material().SetAmbientLightColor(regionColor);
  m_selectionOutline->Material().SetDiffuseLightColor(outlineColor);
  m_selectionOutline->Material().SetAmbientLightColor(outlineColor);
#else
  m_selectionRegionActive->LocalProperties().AlphaMask() = m_alphaMask.Current() * maxSelection;
  m_selectionOutlineActive->LocalProperties().AlphaMask() = m_alphaMask.Current() * maxSelection;
#endif

  prevHandPos = handPos;
}

void ExposeView::updateForces(std::chrono::duration<double> dt) {
  m_forces.clear();
  // activation forces
  static const double MAX_RADIUS_MULT = 1.0;
  static const double FORCE_DISTANCE_MULT = 0.2;
  for (const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if (window->m_layoutLocked)
      continue;

    std::shared_ptr<ImagePrimitive>& img = window->GetTexture();
    if (window->m_hover.Value() > 0.0001f) {
      m_forces.push_back(Force(img->Translation(), (float)(FORCE_DISTANCE_MULT*m_layoutRadius*(window->m_hover.Value() + window->m_activation.Value())), window.get(), (float)(MAX_RADIUS_MULT*m_layoutRadius)));
    }
  }
}

void ExposeView::focusWindow(ExposeViewWindow& window) {
  // TODO:  Perform the requested action:

  // Operation complete, raise the event:
  m_exposeViewEvents(&ExposeViewEvents::onWindowSelected)(window);
}

void ExposeView::updateWindowTextures() {
  for (const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if (window->m_layoutLocked)
      continue;
    window->UpdateTexture();
  }
}

void ExposeView::updateWindowTexturesRoundRobin() {
  static int counter = 0;
  counter++;
  const int num = m_windows.size();
  if (num == 0) {
    return;
  }
  const int selection = counter % num;

  int idx = 0;
  for (const std::shared_ptr<ExposeViewWindow>& window : m_windows) {
    if (window->m_layoutLocked)
      continue;
    if (idx == selection) {
      window->UpdateTexture();
    }
    idx++;
  }
}

Vector2 ExposeView::radialCoordsToPoint(double angle, double distance) {
  return Vector2(distance * std::cos(angle), distance * std::sin(angle));
}

std::shared_ptr<ExposeViewWindow> ExposeView::NewExposeWindow(OSWindow& osWindow) {
  auto retVal = std::shared_ptr<ExposeViewWindow>(new ExposeViewWindow(osWindow));
  m_windows.insert(retVal);

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

  if (!addToExistingGroup(retVal)) {
    createNewGroup(retVal);
  }

  computeLayout();

  return retVal;
}

void ExposeView::computeLayout() {
  Autowired<OSVirtualScreen> fullScreen;
  const Vector2 fullSize(fullScreen->Size().width, fullScreen->Size().height);
  const OSRect fullBounds = fullScreen->Bounds();
  const Vector2 fullOrigin(fullBounds.origin.x, fullBounds.origin.y);
  const Vector2 fullCenter = fullOrigin + 0.5*fullSize;

  auto primaryScreen = fullScreen->PrimaryScreen();
  const Vector2 primarySize(primaryScreen.Size().width, primaryScreen.Size().height);
  const OSRect primaryBounds = primaryScreen.Bounds();
  const Vector2 primaryOrigin(primaryBounds.origin.x, primaryBounds.origin.y);
  const Vector2 primaryCenter = primaryOrigin + 0.5*primarySize;

  const Vector2 primaryToFullScale = primarySize.cwiseQuotient(fullSize);

  const Vector2 aspectScale(primarySize.x() / primarySize.y(), 1.0);

  // find centers and bounding boxes of all groups
  for (const std::shared_ptr<ExposeGroup>& group : m_groups) {
    group->CalculateCenterAndBounds();
  }

  // lay out groups
  int totalNumWindows = 0;
  for (const std::shared_ptr<ExposeGroup>& group : m_groups) {
    totalNumWindows += group->m_groupMembers.size();
  }

  const double groupRadius = 0.25*(0.5*fullSize).norm();
  double groupAngle = 0;
  const double groupAngleInc = 2*M_PI / static_cast<double>(totalNumWindows);
  for (const std::shared_ptr<ExposeGroup>& group : m_groups) {
    const double curAngle = group->m_groupMembers.size()*groupAngleInc;
    groupAngle += 0.5*curAngle;
    const Vector2 cartesian = radialCoordsToPoint(groupAngle, groupRadius).cwiseProduct(aspectScale) + primaryCenter;
    group->m_center = cartesian;
    groupAngle += 0.5*curAngle;
  }

  // lay out group members
  for (const std::shared_ptr<ExposeGroup>& group : m_groups) {
    double totalSize = 0;
    for (const std::shared_ptr<ExposeViewWindow>& window : group->m_groupMembers) {
      totalSize += window->GetOSSize().norm();
    }

    const int numGroupMembers = group->m_groupMembers.size();
    const Vector2 scaledCenter = (group->m_center - primaryCenter).cwiseProduct(primaryToFullScale) + primaryCenter;
    const double radius = groupRadius * 0.05 * (numGroupMembers-1);
    double angle = 0;
    const double angleInc = 2*M_PI / totalSize;
    for (const std::shared_ptr<ExposeViewWindow>& window : group->m_groupMembers) {
      const double curAngle = angleInc * window->GetOSSize().norm();
      angle += 0.5*curAngle;
      const Vector2 cartesian = radialCoordsToPoint(angle, radius).cwiseProduct(aspectScale) + scaledCenter;
      const Vector3 point3D(cartesian.x(), cartesian.y(), 0.0);
      window->m_position.SetGoal(point3D);
      angle += 0.5*curAngle;
    }
  }
}

bool ExposeView::addToExistingGroup(const std::shared_ptr<ExposeViewWindow>& window) {
  std::shared_ptr<ExposeGroup> group = getGroupForWindow(window);
  if (group) {
    group->m_groupMembers.insert(window);
    return true;
  }
  return false;
}

std::shared_ptr<ExposeGroup> ExposeView::getGroupForWindow(const std::shared_ptr<ExposeViewWindow>& window) const {
  OSApp& windowApp = *(window->m_osWindow->GetOwnerApp());
  for (const std::shared_ptr<ExposeGroup>& group : m_groups) {
    OSApp& groupApp = *group->m_app;
    if (windowApp == groupApp) {
      return group;
    }
  }
  return nullptr;
}

std::shared_ptr<ExposeGroup> ExposeView::createNewGroup(const std::shared_ptr<ExposeViewWindow>& window) {
  std::shared_ptr<ExposeGroup> group(new ExposeGroup);
  group->m_app = window->m_osWindow->GetOwnerApp();
  group->m_icon = std::shared_ptr<ImagePrimitive>(new ImagePrimitive);
  group->m_icon = group->m_app->GetIconTexture(group->m_icon);
  group->m_groupMembers.insert(window);
  m_groups.insert(group);
  return group;
}

void ExposeView::RemoveExposeWindow(const std::shared_ptr<ExposeViewWindow>& wnd) {
  m_windows.erase(wnd);
  wnd->RemoveFromParent();

  std::shared_ptr<ExposeGroup> group = getGroupForWindow(wnd);
  group->m_groupMembers.erase(wnd);
  if (group->m_groupMembers.empty()) {
    m_groups.erase(group);
  }

  computeLayout();
}

void ExposeView::UpdateExposeWindow(const std::shared_ptr<ExposeViewWindow>& wnd) {
  wnd->UpdateTexture();

  computeLayout();
}

void ExposeView::StartView() {
  m_alphaMask.Set(1.0f, 0.75);
  AutowiredFast<sf::RenderWindow> mw;
  if (mw) {
    NativeWindow::AllowInput(mw->getSystemHandle(), true);
  }
  m_closing = false;
  startPositions();
  computeLayout();
}

void ExposeView::CloseView() {
  m_alphaMask.Set(0.0f, 0.75);
  AutowiredFast<sf::RenderWindow> mw;
  if (mw) {
    NativeWindow::AllowInput(mw->getSystemHandle(), false);
  }
  m_closing = true;
  endPositions();
}
