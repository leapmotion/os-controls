#include "stdafx.h"
#include "AudioVolumeController.h"

class AudioControllerTest:
  public testing::Test
{};

TEST_F(AudioControllerTest, VerifyVolumeRange) {
  AutoDesired<AudioVolumeController> ac;
  if(!ac)
    return;

  float volume = ac->GetVolume();

  // We don't know what the volume will be (exactly) but we know its bounds
  const char* oob = "Volume value was not reported in the range [0, 1]";
  ASSERT_LE(0.0f, volume) << oob;
  ASSERT_GE(1.0f, volume) << oob;
}

TEST_F(AudioControllerTest, VerifyGetSetMuteVolume) {
  AutoDesired<AudioVolumeController> ac;
  if(!ac)
    return;

  const bool wasMuted = ac->IsMuted();
  if (wasMuted) {
    ac->SetMute(false);
  }
  // Muting should be off
  ASSERT_FALSE(ac->IsMuted()) << "Volume control is expected to be unmuted";
 
  // Get the current volume, mutate it, and verify the mutation is correct
  const float volume = ac->GetVolume();
  // Adjust volume so that it is different
  const float expectedVolume = volume > 0.5f ? volume - 0.5f : volume + 0.5f;
  // Set the expected volume
  ac->SetVolume(expectedVolume);
  // Now see what we actually got back for a volume
  const float actualVolume = ac->GetVolume();
  // Put it back the way it was
  ac->SetVolume(volume);
  // Check to see how far off we actually were
  const float deltaVolume = fabs(expectedVolume - actualVolume);

  // Mute the volume control
  ac->SetMute(true);
  ASSERT_TRUE(ac->IsMuted()) << "Failed to mute volume control";

  if (!wasMuted) {
    // If the volume control was initially unmuted, return it to that state
    ac->SetMute(false);
    ASSERT_FALSE(ac->IsMuted()) << "Failed to unmute volume control";
  }

  // Verify the assignment worked as we expected (within 1%):
  ASSERT_GT(0.01f, deltaVolume) << "Volume assignment did not actually update system volume levels";
}
