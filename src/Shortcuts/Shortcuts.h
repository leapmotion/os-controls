#pragma once
#include <autowiring/autowiring.h>
#include <atomic>

struct ShortcutsContext {};

class AudioVolumeInterface;
class ExposeViewAccessManager;
class LeapInput;
class MediaInterface;
class RenderEngine;
class RenderWindow;

class Shortcuts :
  public ExceptionFilter
{
public:
  Shortcuts(void);
  ~Shortcuts(void);

public:
  void Main(void);
  void Filter(void);
};
