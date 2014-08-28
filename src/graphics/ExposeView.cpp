#include "stdafx.h"
#include "ExposeView.h"
#include "RenderFrame.h"

ExposeView::ExposeView() :
m_handIsGrabbing(false),
m_opacity(0.0f, 0.3, EasingFunctions::Linear<float>)
{
  
}

ExposeView::~ExposeView() {
  
}

void ExposeView::AutoInit() {
  std::cout << "Add ExposeView to the render tree." << std::endl;
  
  m_rootNode.NotifyWhenAutowired([this]{
    auto self = shared_from_this();
    m_rootNode->AddChild(self);
  });
}

void ExposeView::StartView() {
  m_opacity.Set(1.0f, 0.3);
}

void ExposeView::CloseView() {
  m_opacity.Set(0.0f, 0.2f);
}

void ExposeView::AnimationUpdate(const RenderFrame& frame) {
  m_opacity.Update(frame.deltaT.count());
}

void ExposeView::Render(const RenderFrame& frame) const {
  
  for(auto window : m_windows) {
    //QUESTION: Do I need to lock the window here? 
    window->Draw(frame.renderState);
  }
}

void ExposeView::UpdateLayout(uint32_t cursorX, uint32_t cursorY) {
  
}

std::tuple<double, double> radialCoordsToPoint(double angle, double distance) {
  
}

void ExposeView::StartGrab() {
  m_handIsGrabbing = true;
}

bool ExposeView::EndGrab() {
  bool retVal = false;
  m_handIsGrabbing = false;
  return retVal;
}

void ExposeView::moveWindowToTop(std::shared_ptr<RectanglePrim> window) {
  std::vector<std::shared_ptr<RectanglePrim>>::iterator windowIttr = std::find(m_windows.begin(), m_windows.end(), window);
  if ( windowIttr != m_windows.end() ) {
    m_windows.erase(windowIttr);
    m_windows.push_back(window);
  }
}