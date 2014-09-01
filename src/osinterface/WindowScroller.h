#pragma once
#include "ScrollOperation.h"
#include <autowiring/CoreThread.h>

class WindowScrollerEvents {
public:
  /// <summary>
  /// Event raised when scrolling eventually terminates
  /// </summary>
  virtual void OnScrollStopped(void) = 0;
};

class IWindowScroller:
  public CoreThread,
  IScrollOperation
{
public:
  IWindowScroller(void);
  virtual ~IWindowScroller(void);
  static IWindowScroller* New(void);

protected:
  // Events that we raise when interesting things happen
  AutoFired<WindowScrollerEvents> m_wse;

  // The current scroll operation, if one exists.
  std::weak_ptr<IScrollOperation> m_curScrollOp;

  // The point where the most recent scroll operation took place
  OSPoint m_virtualPosition;

  // Total current momentum caused by recently received scroll operations.  This momentum is in
  // scroll units per second, and will be reduced by the drag amount.
  OSPoint m_remainingMomentum;

  // Some of the underlying interfaces do not support floating point scrolling,
  // thus we will keep track of residual scrolling for both pixels and lines.
  OSPoint m_scrollPartialPixel;
  OSPoint m_scrollPartialLine;
  // The pixels-to-line ratio. Actual values will be platform specific.
  OSPoint m_pixelsPerLine;

  /// <summary>
  /// Performs the actual scroll operation requested by the user
  /// </summary>
  virtual void DoScrollBy(float deltaX, float deltaY, bool isMomentum) = 0;

  // IScrollOperation overrides:
  void ScrollBy(const OSPoint& virtualPosition, float deltaX, float deltaY) override final;
  void CancelScroll(void) final;

  // Delay operation
  void OnPerformMomentumScroll(std::chrono::high_resolution_clock::time_point then);

public:
  /// <summary>
  /// Begins a scrolling operation, and provides the caller with a pointer to the operation presently underway
  /// </summary>
  /// <remarks>
  /// This method may potentially return nullptr if a scroll operation is already underway.  In that case,
  /// the caller should not attempt to start a scroll operation.
  /// </remarks>
  std::shared_ptr<IScrollOperation> BeginScroll(void);
};
