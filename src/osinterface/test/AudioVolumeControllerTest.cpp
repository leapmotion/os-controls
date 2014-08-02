#include "stdafx.h"
#include "AudioVolumeController.h"

class AudioControllerTest:
  public testing::Test
{};

TEST_F(AudioControllerTest, VerifyVolumeRange) {
  AutoDesired<AudioVolumeController> ac;
  if(!ac)
    return;

  double volume = ac->GetVolume();

  // We don't know what the volume will be (exactly) but we know its bounds
  const char* oob = "Volume value was not reported in the range [0, 1]";
  ASSERT_LE(0.0, volume) << oob;
  ASSERT_LE(volume, 1.0) << oob;
}

TEST_F(AudioControllerTest, GetSetGet) {
  AutoDesired<AudioVolumeController> ac;
  if(!ac)
    return;
 
  // Get the current volume, mutate it, and verify the mutation is correct
  float volume = ac->GetVolume();
  ac->SetVolume(volume * 0.95);
  float newVolume = ac->GetVolume();

  // Put it back the way it was
  ac->SetVolume(volume);
  
  // Verify the assignment worked as we expected:
  ASSERT_FLOAT_EQ(newVolume, volume * 0.95) << "Volume assignment did not actually update system volume levels";
}