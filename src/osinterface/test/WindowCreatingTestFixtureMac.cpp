#include "stdafx.h"
#include "WindowCreatingTestFixtureMac.h"

WindowCreatingTestFixture* WindowCreatingTestFixture::New(void) {
  return new WindowCreatingTestFixtureMac;
}

TestWindowProperties CreateTestWindow(void) {
  throw std::runtime_error("Not implemented");
}