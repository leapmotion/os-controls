#pragma once

class OSKeyboardEvent {
public:
  virtual void KeyDown(int keycode) {}
  virtual void KeyUp(int keycode) {}
};