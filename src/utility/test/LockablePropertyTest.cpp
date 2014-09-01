#include "stdafx.h"
#include "lockable_property.h"
#include <string>

class LockablePropertyTest:
  public testing::Test
{};

TEST_F(LockablePropertyTest, ValidateMutualExclusion) {
  lockable_property<void> lp;

  {
    auto lk = lp.lock();
    ASSERT_TRUE(lp.is_locked()) << "Property reported it was not locked when it should have been locked";
  }

  ASSERT_FALSE(lp.is_locked()) << "Property was not correctly unlocked";
  ASSERT_TRUE((lp.lock(), lp.is_locked())) << "Lock one-liner did not correctly indicate that the property was locked during expression evaluation";
}

TEST_F(LockablePropertyTest, AssignmentIsExclusive) {
  lockable_property<int> lp;

  {
    auto lk = lp.lock();
    *lk = 5;
    ASSERT_TRUE(lp.is_locked()) << "Atomic property should be locked, but was reported as being unlocked";
  }

  ASSERT_FALSE(lp.is_locked());
  ASSERT_EQ(5, lp.get()) << "Value set during a lock was not correctly persisted";
}
