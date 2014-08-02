// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/gtest-all-guard.h>
#include "utility/ComInitializer.h"

int main(int argc, const char* argv[])
{
  // Will need COM for some tests, might as well initialize it everywhere
  ComInitializer initCom;

  return autotesting_main(argc, argv);
}
