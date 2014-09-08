#include "stdafx.h"
#include "ExposeView.h"
#include "RenderFrame.h"

ExposeView::ExposeView() :
m_handIsGrabbing(false),
m_alphaMask(0.0f, 0.3, EasingFunctions::Linear<float>)
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

//This is called when this class is given control of the app
void ExposeView::StartView() {
  m_alphaMask.Set(1.0f, 0.3);
}

// This is called when the class needs to give up contorl of the app.
// UpdateLayout_TEMP won't be called after this,
// but you will still get a tick in AnimationUpdate and Render.
void ExposeView::CloseView() {
  m_alphaMask.Set(0.0f, 0.2f);
}

// Don't worry about this one
void ExposeView::UpdateLayout(uint32_t cursorX, uint32_t cursorY) {
  std::cout << "ExposeView update layout." << std::endl;
}

// THIS IS ON A TICK
// This will be called as soon as the view is created, which
// Should be a short time after application start.
// This is called whether the expose view has focus or not.
void ExposeView::AnimationUpdate(const RenderFrame& frame) {
  m_alphaMask.Update(frame.deltaT.count());
}

//THIS IS ALSO ON A TICK. RUNS AFTER ANIMATION UPDATE
// This will be called as soon as the view is created, which
// Should be a short time after application start.
// This is called whether the expose view has focus or not.
void ExposeView::Render(const RenderFrame& frame) const {
  
  for(auto window : m_windows) {
    //QUESTION: Do I need to lock the window here?
    window->Draw(frame.renderState);
  }
}

std::tuple<double, double> radialCoordsToPoint(double angle, double distance) {
  std::tuple<double, double> retVal(0,0);
  
  return retVal;
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