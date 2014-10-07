#pragma once
#include <autowiring/autowiring.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <atomic>

struct VRShellContext {};

namespace sf {
  class Event;
  class RenderWindow;
}

int VRShell_main(int argc, char **argv);

class VRShell :
  public ExceptionFilter
{
public:
  VRShell(void);
  ~VRShell(void);

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
