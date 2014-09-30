#pragma once

struct PauseInteractionEvent {
  virtual void PauseChanged(bool enabled) = 0;
};