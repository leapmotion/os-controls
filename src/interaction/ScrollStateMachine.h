#pragma once
#include "ScrollRecognizer.h"
#include "PinchScrollRecognizer.h"

struct ScrollIntention {
  enum class Type {
    None,
    PinchScroll,
    HandScroll
  };

  // Overall conclusion about what kind of scroll operation is presently underway
  Type type;

  // The amount of the scroll intention, in mm
  float scrollAmountX;
  float scrollAmountY;
};

/// <summary>
/// A state machine whose job it is to arbitrary competing scroll directives by the user
/// </summary>
class ScrollStateMachine
{
public:
  ScrollStateMachine(void);
  ~ScrollStateMachine(void);

  void AutoFilter(const HandPinchScroll& hps, const Scroll& scroll, ScrollIntention& intent);

private:

public:
};

