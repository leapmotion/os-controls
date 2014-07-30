#pragma once

#include "SDL.h"

#include <string>

// Design ideas/notes for SDLControllerParams structure
// ----------------------------------------------------
// There are several parameters for configuring/initializing the SDL
// window, renderer and GL context.  A clean way to provide fully
// specified initialization configuration is to pass in a structure
// containing the desired configuration parameters, call it SDLControllerParams.
// There should be sensible defaults for some of the parameters, but
// perhaps some of the parameters have no reasonable defaults and must
// be specified (such as window size).
//
// Such a structure would have the following properties:
// - Params to SDL_Init (currently we use SDL_INIT_TIMER, SDL_INIT_VIDEO, SDL_INIT_EVENTS,
//   but maybe the user also wants SDL_INIT_AUDIO, SDL_INIT_NOPARACHUTE, etc).
// - Transparency on or off
// - Window location
// - Window size
// - Fullscreen on or off
// - Params to SDL_CreateRenderer (currently we use SDL_RENDERER_ACCELERATED,
//   SDL_RENDERER_PRESENTVSYNC, but maybe the user wants SDL_RENDERER_SOFTWARE, which
//   actually would be what we would want on e.g. a unit test of the SDLController
//   and other rendering capabilities).
//
// The default constructor of SDLControllerParams should initialize everything to a
// reasonable default, and SDLController::Initialize should accept a single const
// reference to SDLControllerParams, with a default value of SDLControllerParams().

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

  void MakeTransparent();

  SDL_Window *m_SDL_Window;
  SDL_Renderer *m_SDL_Renderer;
  SDL_GLContext m_SDL_GLContext;
};
