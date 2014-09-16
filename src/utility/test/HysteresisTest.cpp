// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "HysteresisVolume.h"

class HysteresisTest:
public testing::Test
{};

TEST_F(HysteresisTest, SingleTests) {
  Hysteresis increase;
  ASSERT_FALSE(bool(increase)) << "Initialized active by default";
  ASSERT_EQ(0, increase.conditions()) << "Initialized with default conditions";

  increase.increasing = true;
  increase.persistence = 0.;
  increase.activation = 1.;
  ASSERT_EQ(1, increase.conditions()) << "Consistent thresholds did not yield a condition";

  ASSERT_TRUE(bool(increase(2.))) << "Failed to activate";
  ASSERT_TRUE(bool(increase(0.5))) << "Failed to persist";
  ASSERT_EQ(0.5, double(increase)) << "Incorrect interpolation";
  ASSERT_FALSE(bool(increase(-1.))) << "Failed to enervate";
  ASSERT_FALSE(bool(increase(1.))) << "Activation threshold should be exceeded to activate";
  increase.activate();
  ASSERT_TRUE(bool(increase)) << "Failed to activate";
  ASSERT_FALSE(bool(increase(0.))) << "Activation threshold should be matched or exceeded";
  increase.enervate();
  ASSERT_FALSE(bool(increase)) << "Failed to enervate";

  Hysteresis decrease(0., -1., true);
  ASSERT_FALSE(decrease.increasing) << "Incorrect orientation";
  ASSERT_TRUE(bool(decrease)) << "Initialized active by default";
  ASSERT_EQ(1, decrease.conditions()) << "Initialized with default conditions";

  ASSERT_FALSE(bool(decrease(1.))) << "Failed to enervate";
  ASSERT_FALSE(bool(decrease(-0.5))) << "Failed to persist";
  ASSERT_EQ(0.5, double(decrease)) << "Incorrect interpolation";
  ASSERT_TRUE(bool(decrease(-2.))) << "Failed to activate";
  ASSERT_FALSE(bool(decrease(0.))) << "Enervation threshold should be matched or exceeded";
  ASSERT_FALSE(bool(increase(-1.))) << "Activation threshold should be exceeded";

  decrease.increasing = true;
  ASSERT_EQ(0, decrease.conditions()) << "Inconsistent thresholds should yield no conditions";
  ASSERT_FALSE(bool(decrease(-2.))) << "Without conditions there should be no change of state";
  ASSERT_FALSE(bool(decrease(1.))) << "Without conditions there should be no change of state";
}

TEST_F(HysteresisTest, LogicTests) {
  Hysteresis increase(0., 1., true);
  ASSERT_TRUE(bool(increase)) << "Failed to initialize in active state";
  ASSERT_FALSE(bool(increase(-1.))) << "Failed to initialize with conditions";
  ASSERT_TRUE(bool(increase(2.))) << "Failed to initialize with conditions";
  ASSERT_EQ(0.5, double(increase(0.5))) << "Incorrect interpolation";

  //Verify that incompatible orientations cannot be combined
  Hysteresis modifier(0., -1., false);
  ASSERT_FALSE(bool(modifier)) << "Failed to construct in enervated state";
  increase &= modifier;
  ASSERT_TRUE(bool(increase)) << "Incompatible orientation modified state";
  ASSERT_FALSE(bool(increase(-1.))) << "Incompatible orientation modified state";
  ASSERT_TRUE(bool(increase(2.))) << "Incompatible orientation modified state";
  increase |= modifier;
  ASSERT_TRUE(bool(increase)) << "Incompatible orientation modified state";
  ASSERT_FALSE(bool(increase(-1.))) << "Incompatible orientation modified state";
  ASSERT_TRUE(bool(increase(2.))) << "Incompatible orientation modified state";

  //Verify that absent conditions are not applied
  modifier.increasing = true;
  ASSERT_EQ(0, modifier.conditions()) << "Condition with incompatible thresholds";
  increase &= modifier;
  ASSERT_TRUE(bool(increase)) << "Incompatible orientation modified state";
  ASSERT_FALSE(bool(increase(-1.))) << "Incompatible orientation modified state";
  ASSERT_TRUE(bool(increase(2.))) << "Incompatible orientation modified state";

  modifier.persistence = -2.;
  modifier.activation = 2.;
  ASSERT_EQ(1, modifier.conditions()) << "Consistent thresholds failed to yield a condition";
  ASSERT_EQ(0.5, double(modifier(0.))) << "Incorrect interpolation";

  //Verify && combination
  //NOTE: modifier is now activated, with interpolation of 0.5
  //NOTE: increase is now enervated, with interpolation of
  ASSERT_EQ(0.5, double(increase(0.5)));
  increase &= modifier;
  ASSERT_EQ(-2., increase.persistence) << "Persistence threshold was not expanded";
  ASSERT_EQ(2., increase.activation) << "Activation threshold was not expanded";
  ASSERT_FALSE(bool(increase)) << "Enervated && Activated should yield Enervated";
  ASSERT_EQ(0.25, double(increase)) << "0.5 * 0.5 should yield 0.25";

  //Verify || combination
  modifier.persistence = 0.;
  modifier.activation = 1.;
  ASSERT_TRUE(bool(modifier(2.0)));
  ASSERT_EQ(0.5, double(modifier(0.5)));
  increase |= modifier;
  ASSERT_EQ(0., increase.persistence) << "Persistence threshold was not retracted";
  ASSERT_EQ(1., increase.activation) << "Activation threshold was not retracted";
  ASSERT_TRUE(bool(increase)) << "Enervated || Activated should yield Activated";
  ASSERT_EQ(0.625, double(increase)) << "(0.25 + 0.5) - (0.25 * 0.5) should yield 0.625";

  //Verify assignment to threshold without hysteresis
  increase = 0.;
  ASSERT_EQ(0., increase.persistence) << "Persistence threshold was not set";
  ASSERT_EQ(0., increase.activation) << "Activation threshold was not set";

  //Verify displacements
  increase += 1.;
  ASSERT_EQ(1., increase.persistence) << "Persistence threshold was not set";
  ASSERT_EQ(1., increase.activation) << "Activation threshold was not set";
}

TEST_F(HysteresisTest, RangeTest) {
  HysteresisRange range(Hysteresis(0., 1.), Hysteresis(3., 2.));
  ASSERT_EQ(2, range.conditions()) << "Incorrect condition count";
  ASSERT_TRUE(bool(range(1.5))) << "Failed to activate from inlier point";
  ASSERT_EQ(0.5, double(range(0.5))) << "Incorrect interpolation";
  ASSERT_EQ(0.5, double(range(2.5))) << "Incorrect interpolation";

  //Revoke minimum condition
  range.set_min(1., 0.);
  range.enervate();
  ASSERT_FALSE(bool(range)) << "Failed to enervate";
  ASSERT_EQ(1, range.conditions()) << "Incorrect condition count";
  ASSERT_TRUE(range(-1.)) << "Applied revoked condition";

  //Move valid maximum thresholds below revoked minimum
  range.set_max(-1., -2.);
  range.enervate();
  ASSERT_EQ(1, range.conditions()) << "Incorrect condition count";
  ASSERT_TRUE(range(-3.)) << "Applied revoked condition";

  //Reduce to single increasing threshold
  range = Hysteresis(1., 2., false);
  ASSERT_FALSE(bool(range)) << "Failed to assign activation";
  Hysteresis min = range.get_min();
  ASSERT_EQ(1, min.conditions()) << "Min threshold should have one valid condition";
  ASSERT_EQ(1., min.persistence) << "Failed to assign minimum persistence threshold";
  ASSERT_EQ(2., min.activation) << "Failed to assign minimum persistence threshold";
  Hysteresis max = range.get_max();
  ASSERT_EQ(0, max.conditions()) << "Min threshold should have zero valid conditions";
  ASSERT_EQ(1., max.persistence) << "Failed to assign minimum persistence threshold";
  ASSERT_EQ(2., max.activation) << "Failed to assign minimum persistence threshold";
}

TEST_F(HysteresisTest, VolumeTest) {
  // Conversion from HysteresisRange to HysteresisVolume<1>
  HysteresisRange range(Hysteresis(-1., 0.), Hysteresis(2., 1.));
  HysteresisVolume<1> unit1(&range);
  ASSERT_EQ(2, unit1.conditions()) << "Incorrect condition number";
  double point1 = 0.5;
  ASSERT_TRUE(unit1(&point1)) << "Activation failed";
  point1 = 1.5;
  ASSERT_TRUE(unit1(&point1)) << "Persistence failed";

  // Construct cartesian product of ranges
  HysteresisVolume<2> unit2 = unit1 * unit1;
  ASSERT_TRUE(bool(unit2)) << "Combined state should be active";

  double point2[2] = {1.5, 1.5};
  ASSERT_TRUE(unit2(point2)) << "Persistence failed";
  ASSERT_EQ(0.25, double(unit2)) << "Interpolation failed";

  point2[0] = 0.5;
  point2[1] = 3.;
  ASSERT_FALSE(unit2(point2)) << "Eneveration failed";

  // Make second coordinate active when positive
  unit2[1] = Hysteresis(-1., 0.);
  point2[0] = 0.5;
  point2[1] = 3.;
  ASSERT_TRUE(unit2(point2)) << "Failed to activate inside band";
}

