#include "stdafx.h"
#include "CompositionEngine.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <SFML/Graphics.hpp>
#include <chrono>


class CompositionEngineTest :
  public testing::Test
{};

TEST_F(CompositionEngineTest, VerifyBasicVisual) {
  sf::Window window1(sf::VideoMode(640, 480), "Window1");
  window1.setPosition(sf::Vector2i(0, 0));
  sf::RenderWindow window2(sf::VideoMode(640, 480), "Window2");
  window2.setPosition(sf::Vector2i(700, 0));

  AutoRequired<CompositionEngine> engine;
  auto display = engine->CreateDisplay(window1.getSystemHandle());
  auto view = engine->CreateView();

  view->SetContent(window2.getSystemHandle());
  view->SetScale(0.f, 0.f, .2f, .2f);
  view->SetOffset(20, 20);
  view->SetRotation(40, 40, 30);
  display->SetView(view);

  engine->CommitChanges();

  sf::RectangleShape rect(sf::Vector2f(50, 100));
  rect.setPosition(200, 200);
  rect.setFillColor(sf::Color::Blue);
  window2.setFramerateLimit(60);

  auto start = std::chrono::steady_clock::now();
  float rotation = 0;
  while (std::chrono::steady_clock::now() - start < std::chrono::seconds(5)) {
    for (sf::Event evt; window1.pollEvent(evt);) {
    }

    window2.clear();
    window2.setActive();
    rect.rotate(5.0f);
    window2.draw(rect);
    window2.display();
  }

  window1.close();
  window2.close();
}
