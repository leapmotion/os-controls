#pragma once

#include "SDL.h"

#include <string>

// This class bundles all the SDL usage/state into a single point of control.
// TODO: think about renaming this to SDLController or something, because "context"
// means something else in autowiring.
class SDLController {
public:

  SDLController ();
  ~SDLController ();

  // This creates the window, renderer, and GL context.  Throws an exception to denote error.
  void Initialize (bool transparentWindow = false);
  // Shuts everything down, in order opposite of initialization.  No exceptions should be thrown.
  void Shutdown ();

  void BeginRender () const;
  void EndRender () const;

  static std::string BasePath ();

private:

  void makeTransparent();

  SDL_Window *m_SDL_Window;
  SDL_Renderer *m_SDL_Renderer;
  SDL_GLContext m_SDL_GLContext;

};
