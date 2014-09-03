#pragma once
#include <cstdint>

struct OSPoint {
  uint32_t x;
  uint32_t y;

  bool operator==(const OSPoint& rhs) const {
    return x == rhs.x && y == rhs.y;
  }
};