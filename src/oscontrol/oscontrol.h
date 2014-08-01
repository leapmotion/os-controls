#pragma once
#include <autowiring/autowiring.h>

class LeapInput;
class MainWindow;
class SdlInitializer;

class OsControl {
public:
  OsControl(void);

private:
  AutoRequired<SdlInitializer> m_initializer;
  AutoConstruct<MainWindow> m_mw;
  AutoRequired<LeapInput> m_leapInput;

public:
  void Main(void);
};