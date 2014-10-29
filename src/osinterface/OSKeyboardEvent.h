#pragma once

class OSKeyboardEvent {
public:
  virtual void KeyDown(int keycode) = 0;
  virtual void KeyUp(int keycode) = 0;
};