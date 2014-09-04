#pragma once

class ExposeViewWindow;

class ExposeViewEvents {
public:
  virtual ~ExposeViewEvents(void);
  
  /// <summary>
  /// Raised when the user elects to make the specified window a foreground window
  /// </summary>
  /// <param name="wnd">The expose window being brought to the foreground</param>
  virtual void onWindowSelected(ExposeViewWindow& osWindow) {}
};