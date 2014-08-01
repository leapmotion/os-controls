#pragma once
#include <vector>

struct SDL_Window;
class SdlInitializer;

// Interface class for top-level control of an application.
class MainWindow:
  public CoreRunnable
{
public:
  MainWindow(const char* windowTitle);
  ~MainWindow(void);

  // Application interface methods.  See Application for comments and details.
  void Start(void);

private:
  std::mutex m_lock;
  std::condition_variable m_stateCondition;

  const uint32_t m_windowFlags;
  const std::shared_ptr<SDL_Window> m_SDL_Window;

  // Requirements for this to be a CoreRunnable:
  bool m_bShouldStop;
  bool m_bRunning;
  std::shared_ptr<Object> m_outstanding;

  // Need SDL to be initialized before we try to use SDL
  AutoRequired<SdlInitializer> m_initializer;

public:
  /// <summary>
  /// Delegates the main thread to this class for the remaining lifetime of the applicatoin
  /// </summary>
  void DelegatedMain(void);

  // CoreRunnable overrides
  bool Start(std::shared_ptr<Object> outstanding) override;
  void Stop(bool graceful) override { m_bShouldStop = true; }
  bool IsRunning(void) const override { return m_bRunning; }
  bool ShouldStop(void) const override { return m_bShouldStop; }
  void Wait(void) override;
};

static_assert(!std::is_abstract<MainWindow>::value, "Main window cannot be abstract");