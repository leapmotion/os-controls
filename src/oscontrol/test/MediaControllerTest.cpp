#include "stdafx.h"
#include "MediaController.h"

class MediaControllerTest:
  public testing::Test
{};

TEST_F(MediaControllerTest, VerifyVolumeRange) {
  AutoRequired<MediaController> mc;
  double volume = mc->GetVolume();

  // We don't know what the volume will be (exactly) but we know its bounds
  const char* oob = "Volume value was not reported in the range [0, 1]";
  ASSERT_LE(0.0, volume) << oob;
  ASSERT_LE(volume, 1.0) << oob;
}