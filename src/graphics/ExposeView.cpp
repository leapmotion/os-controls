#include "stdafx.h"
#include "ExposeView.h"
#include "ExposeViewWindow.h"
#include "RenderFrame.h"

ExposeView::ExposeView() :
  m_opacity(0.0f, 0.3f, EasingFunctions::Linear<float>)
{
  
}

ExposeView::~ExposeView() {
  
}

void ExposeView::AutoInit() {
  
  m_rootNode.NotifyWhenAutowired([this]{
    auto self = shared_from_this();
    m_rootNode->AddChild(self);
  });
}

void ExposeView::AnimationUpdate(const RenderFrame& frame) {
  m_opacity.Update(frame.deltaT.count());
  UpdateLayout(frame.deltaT);
}

void ExposeView::Render(const RenderFrame& frame) const {
  for(const auto& renderable : m_renderList)
    renderable->Render(frame);
}

void ExposeView::UpdateLayout(std::chrono::duration<double> timeout) {
  // TODO:  Given the current state of this object, update its layout
}

std::tuple<double, double> radialCoordsToPoint(double angle, double distance) {
  return std::make_tuple(0.0, 0.0);
}

std::shared_ptr<ExposeViewWindow> ExposeView::NewExposeWindow(OSWindow& osWindow) {
  auto retVal = std::make_shared<ExposeViewWindow>(osWindow);
  m_windows.push_back(retVal);
  return retVal;
}

void ExposeView::StartView() {
  m_opacity.Set(1.0f, 0.3);
}

void ExposeView::CloseView() {
  m_opacity.Set(0.0f, 0.2f);
}

void ExposeView::moveWindowToTop(ExposeViewWindow& window) {
  std::rotate(
    m_renderList.begin(),
    std::find(m_renderList.begin(), m_renderList.end(), &window),
    m_renderList.end()
  );
}