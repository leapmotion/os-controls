#pragma once
#include <autowiring/autowiring.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <atomic>

struct ShortcutsContext {};

class AudioVolumeInterface;
class ExposeViewAccessManager;
class LeapInput;
class MediaInterface;
class RenderEngine;

namespace sf {
  class Event;
  class RenderWindow;
}

class Shortcuts :
  public ExceptionFilter
{
public:
  Shortcuts(void);
  ~Shortcuts(void);

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
