#pragma once

#include <autowiring/autowiring.h>
#include <gtest/gtest.h>

#ifdef _MSC_VER
  #include <Windows.h>
#endif

static const std::chrono::milliseconds sc_delayTimes [] = {
  std::chrono::milliseconds(100),
  std::chrono::milliseconds(500),
  std::chrono::seconds(1),
  std::chrono::seconds(3),
  std::chrono::seconds(10),
  std::chrono::milliseconds(0)
};