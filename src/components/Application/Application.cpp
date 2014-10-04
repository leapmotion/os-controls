#include "Application.h"

#include "SDL.h"
#include "SDL_quit.h"

void DispatchEventToApplication (const SDL_Event &ev, Application &app) {
  // https://wiki.libsdl.org/SDL_Event?highlight=%28%5CbCategoryStruct%5Cb%29%7C%28SDLStructTemplate%29
  switch (ev.type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:           app.HandleKeyboardEvent(ev.key);        break;

    case SDL_MOUSEMOTION:     app.HandleMouseMotionEvent(ev.motion);  break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:   app.HandleMouseButtonEvent(ev.button);  break;

    case SDL_MOUSEWHEEL:      app.HandleMouseWheelEvent(ev.wheel);    break;

    case SDL_QUIT:            app.HandleQuitEvent(ev.quit);           break;

    case SDL_WINDOWEVENT:     app.HandleWindowEvent(ev.window);       break;

    default:                  app.HandleGenericSDLEvent(ev);          break;
  }
}

void RunApplication (Application &app) {
  // Give the application a chance to initialize itself.
  app.Initialize();

  // Run the game loop until a "quit" has been requested.
  TimePoint previousRealTime(0.001 * SDL_GetTicks());
  do {
    // Handle all queue'd events.
    {
      SDL_Event ev;
      while (SDL_PollEvent(&ev)) {
        DispatchEventToApplication(ev, app);
      }
    }
    // TODO: compute the realtime using std::chrono::time_point and time deltas using std::chrono::duration
    TimePoint currentRealTime(0.001 * SDL_GetTicks());
    TimeDelta real_time_delta(currentRealTime - previousRealTime);

    // Update the application.
    app.Update(real_time_delta);
    // Render the application.
    app.Render(real_time_delta);

    // Save off the updated time for the next loop iteration.
    previousRealTime = currentRealTime;
  } while (!SDL_QuitRequested());

  app.Shutdown();
}
