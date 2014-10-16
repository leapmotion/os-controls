#include "stdafx.h"
#include "CompositionEngine.h"
#include "osinterface/RenderWindow.h"

#include <chrono>
#if __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>

#include <GL/gl.h>
#endif

class CompositionEngineTest :
  public testing::Test
{};

TEST_F(CompositionEngineTest, VerifyBasicVisual) {
  std::shared_ptr<RenderWindow> window1(RenderWindow::New());
  window1->SetRect({{0, 0}, {640, 480}});
  std::shared_ptr<RenderWindow> window2(RenderWindow::New());
  window2->SetRect({{0, 480}, {640, 480}});

  AutoRequired<CompositionEngine> engine;
  auto display = engine->CreateDisplay(window1->GetSystemHandle());
  auto view = engine->CreateView();

  view->SetContent(window2->GetSystemHandle());
  view->SetScale(0.f, 0.f, .2f, .2f);
  view->SetOffset(20, 20);
  view->SetRotation(40, 40, 30);
  display->SetView(view);

  engine->CommitChanges();

  window1->SetTransparent(false);
  window1->SetVisible(true);
  window2->SetTransparent(false);
  window2->SetVSync(true);
  window2->SetVisible(true);

  window2->SetActive(true);
  const auto windowSize = window2->GetSize();
  const float ar = static_cast<float>(windowSize.width)/static_cast<float>(windowSize.height);
  ::glViewport(0, 0, windowSize.width, windowSize.height);
  ::glMatrixMode(GL_PROJECTION);
  ::glLoadIdentity();
  ::glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
  window2->SetActive(false);

  auto start = std::chrono::steady_clock::now();
  float rotation = 0;
  while (std::chrono::steady_clock::now() - start < std::chrono::seconds(5)) {
    window1->ProcessEvents();
    window2->ProcessEvents();

    window2->SetActive(true);

    ::glClearColor(0, 0, 0, 0);
    ::glClear(GL_COLOR_BUFFER_BIT);

    ::glEnable(GL_BLEND);
    ::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);

    ::glPushMatrix();
    ::glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    ::glBegin(GL_QUADS);
      ::glColor3d(1,0,0);
      ::glVertex3f(-1,-1,-5);
      ::glColor3d(1,1,0);
      ::glVertex3f(1,-1,-5);
      ::glColor3d(0,1,0);
      ::glVertex3f(1,1,-5);
      ::glColor3d(0,0,1);
      ::glVertex3f(-1,1,-5);
    ::glEnd();
    ::glPopMatrix();

    rotation += 4.0f;

    window2->FlushBuffer();
    window2->SetActive(false);
  }
}
