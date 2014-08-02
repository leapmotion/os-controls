#include "stdafx.h"
#include "AudioVolumeController.h"

class AudioControllerTest:
  public testing::Test
{};

TEST_F(AudioControllerTest, VerifyVolumeRange) {
  AutoDesired<AudioVolumeController> ac;
  if(!ac)
    // Can't test this functionality right now, early return
    return;

  double volume = ac->GetVolume();

  // We don't know what the volume will be (exactly) but we know its bounds
  const char* oob = "Volume value was not reported in the range [0, 1]";
  ASSERT_LE(0.0, volume) << oob;
  ASSERT_LE(volume, 1.0) << oob;
}