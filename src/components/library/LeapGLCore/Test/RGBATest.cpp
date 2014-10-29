#include <gtest/gtest.h>
#include "Leap/GL/RGBA.h"

class RGBATest : public testing::Test { };

template <typename T>
void TestRGBAEquality () {
  typedef RGBA<T> RGBA;
  static const size_t COUNT = 1000;
  for (size_t i = 0; i < COUNT; ++i) {
    RGBA a(0x123*i, 0x456*i, 0x789*i, 0xABC*i);
    EXPECT_EQ(a, a);
  }
}

TEST_F(RGBATest, RGBAEquality_uint8_t) {
  TestRGBAEquality<uint8_t>();
}

TEST_F(RGBATest, RGBAEquality_uint16_t) {
  TestRGBAEquality<uint16_t>();
}

TEST_F(RGBATest, RGBAEquality_uint32_t) {
  TestRGBAEquality<uint32_t>();
}

TEST_F(RGBATest, RGBAEquality_uint64_t) {
  TestRGBAEquality<uint64_t>();
}

TEST_F(RGBATest, RGBAEquality_float) {
  TestRGBAEquality<float>();
}

TEST_F(RGBATest, RGBAEquality_double) {
  TestRGBAEquality<double>();
}

TEST_F(RGBATest, RGBAEquality_long_double) {
  TestRGBAEquality<long double>();
}

template <typename T>
void TestBlended_RGBA () {
  typedef RGBA<T> RGBA;
  typedef ColorComponent<T> C;
  static const size_t COUNT = 10;
  for (size_t i = 0; i < COUNT; ++i) {
    RGBA a(0x123*i, 0x456*i, 0x789*i, 0xABC*i);
    for (size_t j = 0; j < COUNT; ++j) {
      RGBA b(0x123*j, 0x456*j, 0x789*j, 0xABC*j);
      EXPECT_EQ(a, a.BlendedWith(b, C::Zero()));
      EXPECT_EQ(b, a.BlendedWith(b, C::One()));
    }
  }
}

TEST_F(RGBATest, DISABLED_Blended_RGBA_uint8_t) {
  TestBlended_RGBA<uint8_t>();
}

TEST_F(RGBATest, DISABLED_Blended_RGBA_uint16_t) {
  TestBlended_RGBA<uint16_t>();
}

TEST_F(RGBATest, DISABLED_Blended_RGBA_uint32_t) {
  TestBlended_RGBA<uint32_t>();
}

TEST_F(RGBATest, DISABLED_Blended_RGBA_uint64_t) {
  TestBlended_RGBA<uint64_t>();
}

TEST_F(RGBATest, DISABLED_Blended_RGBA_float) {
  TestBlended_RGBA<float>();
}

TEST_F(RGBATest, DISABLED_Blended_RGBA_double) {
  TestBlended_RGBA<double>();
}

TEST_F(RGBATest, DISABLED_Blended_RGBA_long_double) {
  TestBlended_RGBA<long double>();
}
