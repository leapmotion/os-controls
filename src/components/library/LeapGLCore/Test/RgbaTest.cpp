#include <gtest/gtest.h>
#include "Leap/GL/Rgba.h"

class RgbaTest : public testing::Test { };

template <typename T>
void TestRgbaEquality () {
  typedef Rgba<T> Rgba;
  static const size_t COUNT = 1000;
  for (size_t i = 0; i < COUNT; ++i) {
    Rgba a(0x123*i, 0x456*i, 0x789*i, 0xABC*i);
    EXPECT_EQ(a, a);
  }
}

TEST_F(RgbaTest, RgbaEquality_uint8_t) {
  TestRgbaEquality<uint8_t>();
}

TEST_F(RgbaTest, RgbaEquality_uint16_t) {
  TestRgbaEquality<uint16_t>();
}

TEST_F(RgbaTest, RgbaEquality_uint32_t) {
  TestRgbaEquality<uint32_t>();
}

TEST_F(RgbaTest, RgbaEquality_uint64_t) {
  TestRgbaEquality<uint64_t>();
}

TEST_F(RgbaTest, RgbaEquality_float) {
  TestRgbaEquality<float>();
}

TEST_F(RgbaTest, RgbaEquality_double) {
  TestRgbaEquality<double>();
}

TEST_F(RgbaTest, RgbaEquality_long_double) {
  TestRgbaEquality<long double>();
}

template <typename T>
void TestBlended_Rgba () {
  typedef Rgba<T> Rgba;
  typedef ColorComponent<T> C;
  static const size_t COUNT = 10;
  for (size_t i = 0; i < COUNT; ++i) {
    Rgba a(0x123*i, 0x456*i, 0x789*i, 0xABC*i);
    for (size_t j = 0; j < COUNT; ++j) {
      Rgba b(0x123*j, 0x456*j, 0x789*j, 0xABC*j);
      EXPECT_EQ(a, a.BlendedWith(b, C::Zero()));
      EXPECT_EQ(b, a.BlendedWith(b, C::One()));
    }
  }
}

TEST_F(RgbaTest, Blended_Rgba_uint8_t) {
// TEST_F(RgbaTest, DISABLED_Blended_Rgba_uint8_t) {
  TestBlended_Rgba<uint8_t>();
}

TEST_F(RgbaTest, Blended_Rgba_uint16_t) {
// TEST_F(RgbaTest, DISABLED_Blended_Rgba_uint16_t) {
  TestBlended_Rgba<uint16_t>();
}

TEST_F(RgbaTest, Blended_Rgba_uint32_t) {
// TEST_F(RgbaTest, DISABLED_Blended_Rgba_uint32_t) {
  TestBlended_Rgba<uint32_t>();
}

TEST_F(RgbaTest, Blended_Rgba_uint64_t) {
// TEST_F(RgbaTest, DISABLED_Blended_Rgba_uint64_t) {
  TestBlended_Rgba<uint64_t>();
}

TEST_F(RgbaTest, Blended_Rgba_float) {
// TEST_F(RgbaTest, DISABLED_Blended_Rgba_float) {
  TestBlended_Rgba<float>();
}

TEST_F(RgbaTest, Blended_Rgba_double) {
// TEST_F(RgbaTest, DISABLED_Blended_Rgba_double) {
  TestBlended_Rgba<double>();
}

TEST_F(RgbaTest, Blended_Rgba_long_double) {
// TEST_F(RgbaTest, DISABLED_Blended_Rgba_long_double) {
  TestBlended_Rgba<long double>();
}
