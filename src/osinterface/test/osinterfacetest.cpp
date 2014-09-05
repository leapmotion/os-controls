// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/gtest-all-guard.h>
#include "utility/PlatformInitializer.h"
#include "utility/ResourceManagerInitializer.h"

int main(int argc, const char* argv[])
{
  // On Windows, we will need COM for some tests, might as well initialize it everywhere
  PlatformInitializer init;
  ResourceManagerInitializer resInit(init.BaseAppPath());

  return autotesting_main(argc, argv);
}
