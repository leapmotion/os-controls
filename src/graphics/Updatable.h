#pragma once

class Updatable {
public:
  virtual void Update(double deltaT) = 0;
};