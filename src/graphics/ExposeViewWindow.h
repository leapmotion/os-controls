#pragma once
#include "ExposeView.h"

/// <summary>
/// Accessor interface used to set the activation level
// </summary>
class ExposeViewWindowActivation {
public:
  virtual void SetActivation(float activation) = 0;
};

class MovableWindow {
public:
  virtual void SetPosition(float x, float y) = 0;
};

class ExposeViewWindow:
  ExposeViewWindowActivation,
  MovableWindow
{
public:
  ExposeViewWindow(void);
  ~ExposeViewWindow(void);

private:
  float m_activation;

  std::weak_ptr<ExposeViewWindowActivation> m_activationWeak;
  std::weak_ptr<MovableWindow> m_movableWindowWeak;

public:
  // Activation overrides:
  void SetActivation(float activation) override;

  /// <summary>
  /// Allows the caller to set the activation level
  /// </summary>
  std::shared_ptr<ExposeViewWindowActivation> LockActivation(void);

  /// <summary>
  /// Causes the layout engine to stop attempting to update the layout of this window
  /// </summary>
  std::shared_ptr<MovableWindow> SuspendAutomaticLayout(void);
};