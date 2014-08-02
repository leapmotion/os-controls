#include "stdafx.h"
#include "SdlInitializer.h"
#include <SDL.h>
#include <stdexcept>

SdlInitializer::SdlInitializer(void)
{
  if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    throw std::runtime_error("Failed to start SDL");
}


SdlInitializer::~SdlInitializer(void)
{
  SDL_Quit();
}
