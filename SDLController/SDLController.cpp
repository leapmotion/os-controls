#include "SDLController.h"

#include <cassert>
#include <iostream> // TEMP
#include <stdexcept>

SDLController::SDLController ()
  :
  m_SDL_Window(nullptr),
  m_SDL_Renderer(nullptr),
  m_SDL_GLContext(nullptr)
{
  // TEMP
  std::cerr << "SDLController::BasePath() = \"" << BasePath() << "\"\n";
}

SDLController::~SDLController () {
  assert(m_SDL_Window == nullptr && "did not shut down properly!");
  assert(m_SDL_Renderer == nullptr && "did not shut down properly!");
  assert(m_SDL_GLContext == nullptr && "did not shut down properly!");
}

void SDLController::Initialize () {
  // Initialize SDL.  TODO: initialize audio
  if (SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0) {
    throw std::runtime_error(SDL_GetError());
  }
  // Create a window.
  m_SDL_Window = SDL_CreateWindow("Freeform", 100, 100, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (m_SDL_Window == nullptr) {
    SDL_Quit();
    throw std::runtime_error(SDL_GetError());
  }
  // Create a renderer.  NOTE: it may be platform-specific as to if we want V-sync.
  m_SDL_Renderer = SDL_CreateRenderer(m_SDL_Window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
  if (m_SDL_Renderer == nullptr) {
    SDL_DestroyWindow(m_SDL_Window);
    m_SDL_Window = nullptr;
    SDL_Quit();
    throw std::runtime_error(SDL_GetError());
  }
  // Create a GL context.
  m_SDL_GLContext = SDL_GL_CreateContext(m_SDL_Window);
  if (m_SDL_GLContext == nullptr) {
    SDL_DestroyRenderer(m_SDL_Renderer);
    m_SDL_Renderer = nullptr;
    SDL_DestroyWindow(m_SDL_Window);
    m_SDL_Window = nullptr;
    SDL_Quit();
    throw std::runtime_error(SDL_GetError());
  }
}

void SDLController::Shutdown () {
  // TODO: shutdown anything else necessary, video, and SDL (in reverse order as init)
  if (m_SDL_GLContext != nullptr) {
    SDL_GL_DeleteContext(m_SDL_GLContext);
    m_SDL_GLContext = nullptr;
  }
  if (m_SDL_Renderer != nullptr) {
    SDL_DestroyRenderer(m_SDL_Renderer);
    m_SDL_Renderer = nullptr;
  }
  if (m_SDL_Window != nullptr) {
    SDL_DestroyWindow(m_SDL_Window);
    m_SDL_Window = nullptr;
  }
  SDL_Quit();
}

void SDLController::BeginRender () const {
  // Do whatever needs to be done before rendering a frame.
}

void SDLController::EndRender () const {
  SDL_GL_SwapWindow(m_SDL_Window);
}

std::string SDLController::BasePath () {
  return std::string(SDL_GetBasePath());
}
