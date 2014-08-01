#pragma once
#include <vector>

struct SDL_Window;
class SdlInitializer;

// Interface class for top-level control of an application.
class MainWindow
{
public:
  MainWindow(const char* windowTitle);
  ~MainWindow(void);

  // Application interface methods.  See Application for comments and details.
  void Start(void);

private:

  const uint32_t m_windowFlags;
  const std::shared_ptr<SDL_Window> m_SDL_Window;

  // Need SDL to be initialized before we try to use SDL
  AutoRequired<SdlInitializer> m_initializer;

public:
  /// <summary>
  /// Delegates the main thread to this class for the remaining lifetime of the applicatoin
  /// </summary>
  void DelegatedMain(void);
};

static_assert(!std::is_abstract<MainWindow>::value, "Main window cannot be abstract");