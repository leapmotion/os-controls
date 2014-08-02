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
  m_SDL_Window(SDL_CreateWindow(windowTitle, 0, 0, 150, 150, m_windowFlags), &SDL_DestroyWindow)
{
}

MainWindow::~MainWindow(void) {
}
