#include <gtest/gtest.h>
#include "Leap/GL/RGB.h"

class RGBTest : public testing::Test { };

template <typename T>
void TestRGBEquality () {
  typedef RGB<T> RGB;
  static const size_t COUNT = 1000;
  for (size_t i = 0; i < COUNT; ++i) {
    RGB a(0x123*i, 0x456*i, 0x789*i);
    EXPECT_EQ(a, a);
  }
}

TEST_F(RGBTest, RGBEquality_uint8_t) {
  TestRGBEquality<uint8_t>();
}

TEST_F(RGBTest, RGBEquality_uint16_t) {
  TestRGBEquality<uint16_t>();
}

TEST_F(RGBTest, RGBEquality_uint32_t) {
  TestRGBEquality<uint32_t>();
}

TEST_F(RGBTest, RGBEquality_uint64_t) {
  TestRGBEquality<uint64_t>();
}

TEST_F(RGBTest, RGBEquality_float) {
  TestRGBEquality<float>();
}

TEST_F(RGBTest, RGBEquality_double) {
  TestRGBEquality<double>();
}

TEST_F(RGBTest, RGBEquality_long_double) {
  TestRGBEquality<long double>();
}

template <typename T>
void TestBlended_RGB () {
  typedef RGB<T> RGB;
  typedef ColorComponent<T> C;
  static const size_t COUNT = 10;
  for (size_t i = 0; i < COUNT; ++i) {
    RGB a(0x123*i, 0x456*i, 0x789*i);
    for (size_t j = 0; j < COUNT; ++j) {
      RGB b(0x123*j, 0x456*j, 0x789*j);
      EXPECT_EQ(a, a.BlendedWith(b, C::Zero()));
      EXPECT_EQ(b, a.BlendedWith(b, C::One()));
    }
  }
}

TEST_F(RGBTest, DISABLED_Blended_RGB_uint8_t) {
  TestBlended_RGB<uint8_t>();
}

TEST_F(RGBTest, DISABLED_Blended_RGB_uint16_t) {
  TestBlended_RGB<uint16_t>();
}

TEST_F(RGBTest, DISABLED_Blended_RGB_uint32_t) {
  TestBlended_RGB<uint32_t>();
}

TEST_F(RGBTest, DISABLED_Blended_RGB_uint64_t) {
  TestBlended_RGB<uint64_t>();
}

TEST_F(RGBTest, Blended_RGB_float) {
  TestBlended_RGB<float>();
}

TEST_F(RGBTest, Blended_RGB_double) {
  TestBlended_RGB<double>();
}

TEST_F(RGBTest, DISABLED_Blended_RGB_long_double) {
  TestBlended_RGB<long double>();
}
