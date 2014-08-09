#pragma once

class Drawable;

class Scene
{
public:
  Scene();
  ~Scene();

  void AddDrawable(Drawable* pDrawable);
};

