#pragma once
#include <autowiring/autowiring.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <atomic>

struct OsControlContext {};

class AudioVolumeInterface;
class ExposeViewAccessManager;
class LeapInput;
class MediaInterface;
class RenderEngine;

namespace sf {
  class Event;
  class RenderWindow;
}

int oscontrols_main(int argc, char **argv);

class OsControl :
  public ExceptionFilter
{
public:
  OsControl(void);
  ~OsControl(void);

private:
  Autowired<sf::RenderWindow> m_mw;

  /// <summary>
  /// Handles window & keyboard events from the primary event dispatch loop
  /// </summary>
  void HandleEvent(const sf::Event& ev) const;

public:
  void Main(void);
  void Filter(void);
};
