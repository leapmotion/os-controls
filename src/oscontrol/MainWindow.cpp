#include "stdafx.h"
#include "MainWindow.h"
#include "SdlInitializer.h"
#include <SDL.h>

MainWindow::MainWindow(const char* windowTitle):
  m_windowFlags(
    SDL_WINDOW_OPENGL |
    SDL_WINDOW_SHOWN |

    // Transparent window only works properly when the window is borderless
    SDL_WINDOW_BORDERLESS
  ),
  m_SDL_Window(SDL_CreateWindow(windowTitle, 0, 0, 0, 0, m_windowFlags), &SDL_DestroyWindow)
{
}

MainWindow::~MainWindow(void) {
}

void MainWindow::DelegatedMain(void) {
  auto clearOutstanding = MakeAtExit([this] {
    std::lock_guard<std::mutex> lk(m_lock);
    m_outstanding.reset();
    m_stateCondition.notify_all();
  });

  while(!ShouldStop()) {
    ;
  }
}

bool MainWindow::Start(std::shared_ptr<Object> outstanding) {
  std::lock_guard<std::mutex> lk(m_lock);
  if(m_bShouldStop)
    return true;
  m_outstanding = outstanding;
  return true;
}

void MainWindow::Wait(void) {
  std::unique_lock<std::mutex> lk(m_lock);
  m_stateCondition.wait(lk, [this] { return m_outstanding.get() == nullptr; });
}