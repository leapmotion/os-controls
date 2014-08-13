// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include <gtest/gtest-all.cc>
#include <gtest/gtest_macro.h>

GTEST_INCLUDE(cpp11)

using namespace std;

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
