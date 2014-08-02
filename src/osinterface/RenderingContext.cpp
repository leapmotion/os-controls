#include "stdafx.h"
#include "RenderingContext.h"
#include <SDL.h>

RenderingContext::RenderingContext(bool useAlpha)
{
  // Set some SDL attributes. Must be done before creating GL context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);

  if(useAlpha) {
    // Must allocate an alpha channel when using transparent window
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  }
}

RenderingContext::~RenderingContext(void)
{
}
