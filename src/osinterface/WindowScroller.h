#pragma once
#include "ScrollOperation.h"
#include <autowiring/CoreThread.h>
#include "Animation.h"

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

private:
  // Events that we raise when interesting things happen
  AutoFired<WindowScrollerEvents> m_wse;

  // The current scroll operation, if one exists.
  std::weak_ptr<IScrollOperation> m_curScrollOp;

protected:
  /// <summary>
  /// Performs the actual scroll operation requested by the user
  /// </summary>
  virtual void DoScrollBy(float deltaX, float deltaY, bool isMomentum) = 0;

  // IScrollOperation overrides:
  void ScrollBy(float deltaX, float deltaY) override final;
  void CancelScroll(void) override final;

private:
  // Use our own mutex (instead of GetLock()) to prevent possible deadlock
  std::mutex m_mutex;

  // Each scroll sequence has its own unique ID
  uint32_t m_scrollId;

  // Total current momentum caused by recently received scroll operations.  This momentum is in
  // scroll units per microsecond, and will be reduced by the drag amount.
  OSPoint m_remainingMomentum;

  Smoothed<float, 15> m_VelocityX;
  Smoothed<float, 15> m_VelocityY;

  // Time-point of last scroll
  std::chrono::steady_clock::time_point m_lastScrollTimePoint;

  // Periodically apply momentum scroll after normal scrolling has completed
  void OnPerformMomentumScroll(uint32_t scrollId);

  // Reset scrolling, regardless of whether or not we are applying momentum
  void ResetScrollingUnsafe();

public:
  /// <summary>
  /// Set pixels-per-inch to use when scrolling
  /// </summary>
  void SetPixelsPerInch(float ppi);

  /// <summary>
  /// Begins a scrolling operation, and provides the caller with a pointer to the operation presently underway
  /// </summary>
  /// <remarks>
  /// This method may potentially return nullptr if a scroll operation is already underway.  In that case,
  /// the caller should not attempt to start a scroll operation.
  /// </remarks>
  std::shared_ptr<IScrollOperation> BeginScroll(void);

  /// <summary>
  /// If a momentum operation is underway, prevents it from continuing, otherwise has no effect
  /// </summary>
  void StopMomentumScrolling(void);
};
