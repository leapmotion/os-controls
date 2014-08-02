#pragma once
#include <autowiring/autowiring.h>

class AudioVolumeController;
class LeapInput;
class MainWindow;
class MediaController;
class SdlInitializer;
union SDL_Event;

int osinterface_main(int argc, char **argv);

class OsControl:
  public CoreRunnable,
  public ExceptionFilter
{
public:
  OsControl(void);

private:
  std::mutex m_lock;
  std::condition_variable m_stateCondition;

  AutoRequired<SdlInitializer> m_initializer;
  AutoDesired<AudioVolumeController> m_avcontrol;
  AutoConstruct<MainWindow> m_mw;
  AutoRequired<MediaController> m_media;
  AutoRequired<LeapInput> m_leapInput;

  // Requirements for this to be a CoreRunnable:
  bool m_bShouldStop;
  bool m_bRunning;
  std::shared_ptr<Object> m_outstanding;

  /// <summary>
  /// Handles an SDL event from the primary event dispatch loop
  /// </summary>
  void HandleEvent(const SDL_Event& ev) const;

public:
  void Main(void);
  void Filter(void);

  // CoreRunnable overrides
  bool Start(std::shared_ptr<Object> outstanding) override;
  void Stop(bool graceful) override { m_bShouldStop = true; }
  bool IsRunning(void) const override { return m_bRunning; }
  bool ShouldStop(void) const override { return m_bShouldStop; }
  void Wait(void) override;
};