#pragma once
#include "HandPinchRecognizer.h"

struct HandPinchScroll {};

/// <summary>
/// Attempts to recognize a "scroll" intention given a pinch gesture
/// </summary>
/// <remarks>
/// This type attempts to determine whether a user intended to scroll given a recognized pinch gesture.
/// </remarks>
class PinchScrollRecognizer
{
public:
  PinchScrollRecognizer();
  ~PinchScrollRecognizer();

  void AutoFilter(const HandPinch& handPinch, HandPinchScroll& handPinchScroll);
};

