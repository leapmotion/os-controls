#include "stdafx.h"
#include "oscontrol.h"
#include <autowiring/autowiring.h>
#include <SDL.h>

int main(int argc, char **argv)
{
  if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    return -1;

  auto cleanupSDL = MakeAtExit(&SDL_Quit);

  try {
    MainWindow app;
  }
  catch(...) {
    return -1;
  }

  return 0;
}
