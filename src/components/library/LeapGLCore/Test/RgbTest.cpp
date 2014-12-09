#include <gtest/gtest.h>
#include "Leap/GL/Rgb.h"

using namespace Leap::GL;

class RgbTest : public testing::Test { };

template <typename T>
void TestRgbEquality () {
  typedef Rgb<T> Rgb;
  static const size_t COUNT = 1000;
  for (size_t i = 0; i < COUNT; ++i) {
    Rgb a(static_cast<T>(0x123*i), static_cast<T>(0x456*i), static_cast<T>(0x789*i));
    EXPECT_EQ(a, a);
  }
}

TEST_F(RgbTest, RgbEquality_uint8_t) {
  TestRgbEquality<uint8_t>();
}

TEST_F(RgbTest, RgbEquality_uint16_t) {
  TestRgbEquality<uint16_t>();
}

TEST_F(RgbTest, RgbEquality_uint32_t) {
  TestRgbEquality<uint32_t>();
}

TEST_F(RgbTest, RgbEquality_uint64_t) {
  TestRgbEquality<uint64_t>();
}

TEST_F(RgbTest, RgbEquality_float) {
  TestRgbEquality<float>();
}

TEST_F(RgbTest, RgbEquality_double) {
  TestRgbEquality<double>();
}

TEST_F(RgbTest, RgbEquality_long_double) {
  TestRgbEquality<long double>();
}

template <typename T>
void TestBlended_Rgb () {
  typedef Rgb<T> Rgb;
  typedef ColorComponent<T> C;

  static const size_t SAMPLE_COUNT = 33;
  Rgb delta = Rgb::One();
  delta.R() /= (SAMPLE_COUNT-1);
  delta.G() /= (SAMPLE_COUNT-1);
  delta.B() /= (SAMPLE_COUNT-1);
  size_t i, j;
  Rgb value_i, value_j;
  for (i = 0, value_i = Rgb::Zero(); i < SAMPLE_COUNT; ++i, value_i += delta) {
    for (j = 0, value_j = Rgb::Zero(); j < SAMPLE_COUNT; ++j, value_j += delta) {
      EXPECT_EQ(value_i, value_i.BlendedWith(value_j, C::Zero()));
      EXPECT_EQ(value_j, value_i.BlendedWith(value_j, C::One()));
    }
  }
}

TEST_F(RgbTest, Blended_Rgb_uint8_t) {
  TestBlended_Rgb<uint8_t>();
}

TEST_F(RgbTest, Blended_Rgb_uint16_t) {
  TestBlended_Rgb<uint16_t>();
}

TEST_F(RgbTest, Blended_Rgb_uint32_t) {
  TestBlended_Rgb<uint32_t>();
}

TEST_F(RgbTest, Blended_Rgb_uint64_t) {
  TestBlended_Rgb<uint64_t>();
}

TEST_F(RgbTest, Blended_Rgb_float) {
  TestBlended_Rgb<float>();
}

TEST_F(RgbTest, Blended_Rgb_double) {
  TestBlended_Rgb<double>();
}

TEST_F(RgbTest, DISABLED_Blended_Rgb_long_double) {
  TestBlended_Rgb<long double>();
}
