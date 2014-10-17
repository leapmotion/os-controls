#include "SystemWipeRecognizer.h"

#include <cmath>
#include <cstdint>
#include <vector>

#include <iostream> // TEMP

void SystemWipeRecognizer::AutoFilter(const Leap::Frame& frame, SystemWipe& systemWipe) {
  systemWipe.isWiping = false;

  Leap::ImageList images = frame.images();
  Leap::Image left_image = images[0];
  std::cout << "left image isValid = " << left_image.isValid() << '\n';
  if (!left_image.isValid()) {
    return;
  }

  std::vector<float> brightness;
  size_t sample_count = 20;
  float vertical_angle_begin = 20.0f;
  float vertical_angle_end = -20.0f;
  for (float vertical_angle = vertical_angle_begin;
       vertical_angle >= vertical_angle_end;
       vertical_angle -= std::abs(vertical_angle_end-vertical_angle_begin) / (sample_count-1))
  {
    float horizontal_slope = 0.0f;
    float vertical_slope = tan(vertical_angle * 180.0f/Leap::PI);
    Leap::Vector pixel = left_image.warp(Leap::Vector(horizontal_slope, vertical_slope, 0.0f));
    int data_index = std::floor(pixel.y) * left_image.width() + std::floor(pixel.x);
    brightness.push_back(left_image.data()[data_index] / 255.0f);
  }
  std::cout << "SystemWipeRecognizer brightness:\n";
  for (auto b : brightness) {
    std::cout << "    " << b << '\n';
  }
  std::cout << '\n';
}
