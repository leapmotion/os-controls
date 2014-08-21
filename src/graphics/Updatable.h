#pragma once

class Updatable {
public:
  /// <summary>
  /// A time-computed update notification, invoked periodically to notify listeners of the passage of time
  /// </summary>
  virtual void Update(double deltaT) = 0;
};