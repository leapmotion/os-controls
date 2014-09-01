#pragma once
#include "osinterface/LeapInputListener.h"
#include <unordered_map>

class CoreContext;

namespace Leap {
  class Frame;
  class Hand;
}

/// <summary>
/// Fragments input frames out into multiple processing contexts
/// </summary>
class FrameFragmenter:
  public LeapInputListener
{
public:
  FrameFragmenter(void);
  ~FrameFragmenter(void);

  // LeapInputListener overrides
  void OnLeapFrame(const Leap::Frame& frame) override;

private:
  // The processing contexts as known by the system right now
  std::unordered_map<int, std::shared_ptr<CoreContext>> m_contexts;

  /// <summary>
  /// Creates a new processing context to handle operations on a particular hand
  /// </summary>
  std::shared_ptr<CoreContext> CreateMenuContext(const Leap::Hand& hand) const;
};

