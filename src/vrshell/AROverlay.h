#pragma once
#include "LeapImagePassthrough.h"
#include "interaction/SystemWipeRecognizer.h"
#include "osinterface/CompositionEngine.h"
#include "osinterface/OSKeyboardEvent.h"
#include "osinterface/RenderWindow.h"
#include "uievents/Updatable.h"
#include "Animation.h"

#include <autowiring/Autowired.h>

//Takes a main window, hides it, and renders it as a fullscreen overlay
class AROverlay :
  public Updatable,
  public OSKeyboardEvent
{
public:
  AROverlay();
  virtual ~AROverlay();

  //if copyDimensions is true, this will make the overlay appear in the same place
  //as the source window.
  void SetSourceWindow(RenderWindow& window, bool copyDimensions = true);
  void SetOverlayWindowRect(float x, float y, float width, float height);

  WindowHandle GetOverlayWindowHandle() const { return m_overlayWindow->GetSystemHandle(); }

  //Updateable overrides
  void Tick(std::chrono::duration<double> deltaT) override;

  //OSKeyboardEvent overrides
  void KeyDown(int keycode) override;

  //AutoFilter methods (to be informed of system wipe occuring
  void AutoFilter(const SystemWipe& wipe, const Leap::Frame& frame);
private:
  bool isDisplayingOverlay() { return m_overlayOffset.Goal() != 0; }

  bool m_wipeDisabled;
  SystemWipe m_lastWipe;
  SystemWipe::Direction m_wipeDirection;
  std::chrono::steady_clock::time_point m_wipeStart;

  Animated<float> m_overlayOffset;

  std::unique_ptr<RenderWindow> m_overlayWindow;
  std::unique_ptr<ComposedDisplay> m_compDisplay;
  std::unique_ptr<ComposedView> m_mainView;

  AutoRequired<CompositionEngine> m_compEngine;
  AutoRequired<SystemWipeRecognizer> m_recognizer;
};

