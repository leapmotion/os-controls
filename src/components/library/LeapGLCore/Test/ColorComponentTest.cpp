#include <gtest/gtest.h>
#include "Leap/GL/ColorComponent.h"
#include <limits>

using namespace Leap::GL;

class ColorComponentTest : public testing::Test { };

template <typename T_>
void TestComponentZeroAndOne (const T_ &zero_value, const T_ &one_value) {
  auto zero = ColorComponent<T_>::Zero();
  auto one = ColorComponent<T_>::One();
  EXPECT_EQ(zero_value, zero);
  EXPECT_EQ(one_value, one);
  EXPECT_EQ(zero, zero+zero);
  EXPECT_EQ(one, one+zero);
  EXPECT_EQ(one, zero+one);
  EXPECT_EQ(zero, zero*zero);
  EXPECT_EQ(zero, zero*one);
  EXPECT_EQ(zero, one*zero);
  EXPECT_EQ(one, one*one);
  static const size_t SAMPLE_COUNT = 33;
  const T_ delta = one_value / (SAMPLE_COUNT-1);
  ColorComponent<T_> value(zero_value);
  for (size_t i = 0; i < SAMPLE_COUNT; ++i, value.Value() += delta) {
    EXPECT_EQ(value, zero+value);
    EXPECT_EQ(value, value+zero);
    EXPECT_EQ(zero, zero*value);
    EXPECT_EQ(zero, value*zero);
    EXPECT_EQ(value, one*value);
    EXPECT_EQ(value, value*one);
  }
}

TEST_F(ColorComponentTest, ZeroAndOne) {
  TestComponentZeroAndOne<uint8_t>(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());
  TestComponentZeroAndOne<uint16_t>(std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());
  TestComponentZeroAndOne<uint32_t>(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());
  TestComponentZeroAndOne<uint64_t>(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
  TestComponentZeroAndOne<float>(0.0f, 1.0f);
  TestComponentZeroAndOne<double>(0.0, 1.0);
  TestComponentZeroAndOne<long double>(0.0, 1.0);
}

template <typename From, typename To>
void TestConvertComponent_ConversionEndpointMapping () {
  // Ensure that the endpoints of the dynamic range are mapped. 
  EXPECT_EQ(ColorComponent<From>::Zero().template AsComponent<To>(),
            ColorComponent<To>::Zero());
  EXPECT_EQ(ColorComponent<From>::One().template AsComponent<To>(),
            ColorComponent<To>::One());
}

TEST_F(ColorComponentTest, ConversionEndpointMapping) {
  TestConvertComponent_ConversionEndpointMapping<uint8_t,uint8_t>();
  TestConvertComponent_ConversionEndpointMapping<uint8_t,uint16_t>();
  TestConvertComponent_ConversionEndpointMapping<uint8_t,uint32_t>();
  TestConvertComponent_ConversionEndpointMapping<uint8_t,uint64_t>();
  TestConvertComponent_ConversionEndpointMapping<uint8_t,float>();
  TestConvertComponent_ConversionEndpointMapping<uint8_t,double>();
  TestConvertComponent_ConversionEndpointMapping<uint8_t,long double>();

  TestConvertComponent_ConversionEndpointMapping<uint16_t,uint8_t>();
  TestConvertComponent_ConversionEndpointMapping<uint16_t,uint16_t>();
  TestConvertComponent_ConversionEndpointMapping<uint16_t,uint32_t>();
  TestConvertComponent_ConversionEndpointMapping<uint16_t,uint64_t>();
  TestConvertComponent_ConversionEndpointMapping<uint16_t,float>();
  TestConvertComponent_ConversionEndpointMapping<uint16_t,double>();
  TestConvertComponent_ConversionEndpointMapping<uint16_t,long double>();

  TestConvertComponent_ConversionEndpointMapping<uint32_t,uint8_t>();
  TestConvertComponent_ConversionEndpointMapping<uint32_t,uint16_t>();
  TestConvertComponent_ConversionEndpointMapping<uint32_t,uint32_t>();
  TestConvertComponent_ConversionEndpointMapping<uint32_t,uint64_t>();
  TestConvertComponent_ConversionEndpointMapping<uint32_t,float>();
  TestConvertComponent_ConversionEndpointMapping<uint32_t,double>();
  TestConvertComponent_ConversionEndpointMapping<uint32_t,long double>();

  TestConvertComponent_ConversionEndpointMapping<uint64_t,uint8_t>();
  TestConvertComponent_ConversionEndpointMapping<uint64_t,uint16_t>();
  TestConvertComponent_ConversionEndpointMapping<uint64_t,uint32_t>();
  TestConvertComponent_ConversionEndpointMapping<uint64_t,uint64_t>();
  TestConvertComponent_ConversionEndpointMapping<uint64_t,float>();
  TestConvertComponent_ConversionEndpointMapping<uint64_t,double>();
  TestConvertComponent_ConversionEndpointMapping<uint64_t,long double>();

  TestConvertComponent_ConversionEndpointMapping<float,uint8_t>();
  TestConvertComponent_ConversionEndpointMapping<float,uint16_t>();
  TestConvertComponent_ConversionEndpointMapping<float,uint32_t>();
  TestConvertComponent_ConversionEndpointMapping<float,uint64_t>();
  TestConvertComponent_ConversionEndpointMapping<float,float>();
  TestConvertComponent_ConversionEndpointMapping<float,double>();
  TestConvertComponent_ConversionEndpointMapping<float,long double>();

  TestConvertComponent_ConversionEndpointMapping<double,uint8_t>();
  TestConvertComponent_ConversionEndpointMapping<double,uint16_t>();
  TestConvertComponent_ConversionEndpointMapping<double,uint32_t>();
  TestConvertComponent_ConversionEndpointMapping<double,uint64_t>();
  TestConvertComponent_ConversionEndpointMapping<double,float>();
  TestConvertComponent_ConversionEndpointMapping<double,double>();
  TestConvertComponent_ConversionEndpointMapping<double,long double>();

  TestConvertComponent_ConversionEndpointMapping<long double,uint8_t>();
  TestConvertComponent_ConversionEndpointMapping<long double,uint16_t>();
  TestConvertComponent_ConversionEndpointMapping<long double,uint32_t>();
  TestConvertComponent_ConversionEndpointMapping<long double,uint64_t>();
  TestConvertComponent_ConversionEndpointMapping<long double,float>();
  TestConvertComponent_ConversionEndpointMapping<long double,double>();
  TestConvertComponent_ConversionEndpointMapping<long double,long double>();
}

template <typename From, typename To>
void TestConvertComponent_LosslessIntegralConversion () {
  static_assert(std::is_integral<From>::value &&
                std::is_integral<To>::value &&
                sizeof(From) <= sizeof(To),
                "This test is appropriate only for lossless integral conversions");

  // Ensure that the values in the vicinity of each are mapped in a linearly interpolated way.
  ColorComponent<From> one(1);
  static const int SAMPLE_COUNT = 100000;
  {
    ColorComponent<From> i(0);
    for (int sample = 0; sample < SAMPLE_COUNT; ++sample, ++i.Value()) {
      EXPECT_EQ(static_cast<To>(i)*one.template AsComponent<To>(), i.template AsComponent<To>());
    }
  }
  {
    ColorComponent<From> i(static_cast<From>(-SAMPLE_COUNT));
    for (int sample = 0; sample < SAMPLE_COUNT; ++sample, ++i.Value()) {
      EXPECT_EQ(static_cast<To>(i)*one.template AsComponent<To>(), i.template AsComponent<To>());
    }
  }
}

TEST_F(ColorComponentTest, ConvertComponent_LosslessIntegral) {
  TestConvertComponent_LosslessIntegralConversion<uint8_t,uint8_t>();
  TestConvertComponent_LosslessIntegralConversion<uint8_t,uint16_t>();
  TestConvertComponent_LosslessIntegralConversion<uint8_t,uint32_t>();
  TestConvertComponent_LosslessIntegralConversion<uint8_t,uint64_t>();

  TestConvertComponent_LosslessIntegralConversion<uint16_t,uint16_t>();
  TestConvertComponent_LosslessIntegralConversion<uint16_t,uint32_t>();
  TestConvertComponent_LosslessIntegralConversion<uint16_t,uint64_t>();

  TestConvertComponent_LosslessIntegralConversion<uint32_t,uint32_t>();
  TestConvertComponent_LosslessIntegralConversion<uint32_t,uint64_t>();

  TestConvertComponent_LosslessIntegralConversion<uint64_t,uint64_t>();
}

template <typename From, typename To>
void TestConvertComponent_PartialInverse () {
  static const int SAMPLE_COUNT = 100; //100000;
  // Ensure that converting to a component type at least as large has a perfect inverse.
  {
    ColorComponent<From> i(0);
    for (int sample = 0; sample < SAMPLE_COUNT; ++sample, ++i.Value()) {
      EXPECT_EQ(i, i.template AsComponent<To>().template AsComponent<From>());
    }
  }
  {
    ColorComponent<From> i(static_cast<From>(-SAMPLE_COUNT));
    for (int sample = 0; sample < SAMPLE_COUNT; ++sample, ++i.Value()) {
      EXPECT_EQ(i, i.template AsComponent<To>().template AsComponent<From>());
    }
  }
}

TEST_F(ColorComponentTest, ConvertComponent_PartialInverse) {
  TestConvertComponent_PartialInverse<uint8_t,uint8_t>();
  TestConvertComponent_PartialInverse<uint8_t,uint16_t>();
  TestConvertComponent_PartialInverse<uint8_t,uint32_t>();
  TestConvertComponent_PartialInverse<uint8_t,uint64_t>();
  TestConvertComponent_PartialInverse<uint8_t,float>();
  TestConvertComponent_PartialInverse<uint8_t,double>();
  TestConvertComponent_PartialInverse<uint8_t,long double>();

  TestConvertComponent_PartialInverse<uint16_t,uint16_t>();
  TestConvertComponent_PartialInverse<uint16_t,uint32_t>();
  TestConvertComponent_PartialInverse<uint16_t,uint64_t>();
  TestConvertComponent_PartialInverse<uint16_t,float>();
  TestConvertComponent_PartialInverse<uint16_t,double>();
  TestConvertComponent_PartialInverse<uint16_t,long double>();

  TestConvertComponent_PartialInverse<uint32_t,uint32_t>();
  TestConvertComponent_PartialInverse<uint32_t,uint64_t>();
  TestConvertComponent_PartialInverse<uint32_t,double>();
  TestConvertComponent_PartialInverse<uint32_t,long double>();

  TestConvertComponent_PartialInverse<uint64_t,uint64_t>();
  // Maybe, depending on if long double actually has at least 63 (+1 implicit) mantissa bits.
  TestConvertComponent_PartialInverse<uint64_t,long double>();

  TestConvertComponent_PartialInverse<float,float>();
  TestConvertComponent_PartialInverse<float,double>();
  TestConvertComponent_PartialInverse<float,long double>();

  TestConvertComponent_PartialInverse<double,double>();
  TestConvertComponent_PartialInverse<double,long double>();

  TestConvertComponent_PartialInverse<long double,long double>();
}

template <typename From, typename To>
void TestConvertComponent_LossyIntegralConversion () {
  // TODO: test a bunch of other values -- need to think about rounding
}

TEST_F(ColorComponentTest, ConvertComponent_LossyIntegralConversion) {
  TestConvertComponent_LossyIntegralConversion<uint64_t,uint8_t>();
  TestConvertComponent_LossyIntegralConversion<uint64_t,uint16_t>();
  TestConvertComponent_LossyIntegralConversion<uint64_t,uint32_t>();

  TestConvertComponent_LossyIntegralConversion<uint32_t,uint8_t>();
  TestConvertComponent_LossyIntegralConversion<uint32_t,uint16_t>();

  TestConvertComponent_LossyIntegralConversion<uint16_t,uint8_t>();
}

template <typename T_>
void TestBlended_Component () {
  typedef ColorComponent<T_> C;

  static const size_t SAMPLE_COUNT = 33;
  const T_ delta = C::One() / (SAMPLE_COUNT-1);
  size_t i, j;
  C value_i, value_j;
  for (i = 0, value_i = C::Zero(); i < SAMPLE_COUNT; ++i, value_i.Value() += delta) {
    for (j = 0, value_j = C::Zero(); j < SAMPLE_COUNT; ++j, value_j.Value() += delta) {
      EXPECT_EQ(value_i, value_i.BlendedWith(value_j, C::Zero()));
      EXPECT_EQ(value_j, value_i.BlendedWith(value_j, C::One()));
    }
  }
}

TEST_F(ColorComponentTest, Blended_Component_uint8_t) {
  TestBlended_Component<uint8_t>();
}

TEST_F(ColorComponentTest, Blended_Component_uint16_t) {
  TestBlended_Component<uint16_t>();
}

TEST_F(ColorComponentTest, Blended_Component_uint32_t) {
  TestBlended_Component<uint32_t>();
}

TEST_F(ColorComponentTest, Blended_Component_uint64_t) {
  TestBlended_Component<uint64_t>();
}

TEST_F(ColorComponentTest, Blended_Component_float) {
  TestBlended_Component<float>();
}

TEST_F(ColorComponentTest, Blended_Component_double) {
  TestBlended_Component<double>();
}

TEST_F(ColorComponentTest, Blended_Component_long_double) {
  TestBlended_Component<long double>();
}
