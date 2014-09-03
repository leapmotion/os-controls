#pragma once

#include "OSGeometry.h"

class IScrollOperation {
public:
  virtual ~IScrollOperation(void) {}

  /// <summary>
  /// Causes a scroll by the specified amount
  /// </summary>
  virtual void ScrollBy(float deltaX, float deltaY) = 0;

  /// <summary>
  /// Prevents the scroll operation from attempting to generate momentum when it completes
  /// </summary>
  /// <remarks>
  /// Cancel may be called at any time.  Once a call to Cancel has been made, further calls
  /// to ScrollBy will have no effect.  The user should release this object as soon as
  /// possible after Cancel has been called.
  ///
  /// A call to Cancel will also permit another scroll operation to be started.
  /// </remarks>
  virtual void CancelScroll(void) = 0;
};
