#include "stdafx.h"
#include "MediaInterface.h"
#include "AudioVolumeInterface.h"

#include <chrono>
#include <thread>

class MediaInterfaceTest:
  public testing::Test
{};

TEST_F(MediaInterfaceTest, VerifyVolumeControls) {
  AutoDesired<AudioVolumeInterface> ac;
  AutoDesired<MediaInterface> mc;
  if(!ac || !mc)
    return;

  // We will delay for a short while to make sure that the volume is handled by the OS
  std::chrono::milliseconds delay(100); // 100 msec

  float originalVolume = ac->GetVolume(), intermediateVolume, finalVolume;
  if (originalVolume >= 0.5f) {
    mc->VolumeDown();
    std::this_thread::sleep_for(delay);
    intermediateVolume = ac->GetVolume();
    ASSERT_LT(intermediateVolume, originalVolume);
    mc->VolumeUp();
    std::this_thread::sleep_for(delay);
    finalVolume = ac->GetVolume();
    ASSERT_GT(finalVolume, intermediateVolume);
  } else {
    mc->VolumeUp();
    std::this_thread::sleep_for(delay);
    intermediateVolume = ac->GetVolume();
    ASSERT_GT(intermediateVolume, originalVolume);
    mc->VolumeDown();
    std::this_thread::sleep_for(delay);
    finalVolume = ac->GetVolume();
    ASSERT_LT(finalVolume, intermediateVolume);
  }
  bool wasMuted = ac->IsMuted();
  mc->Mute();
  std::this_thread::sleep_for(delay);
  ASSERT_NE(wasMuted, ac->IsMuted());
  mc->Mute();
  std::this_thread::sleep_for(delay);
  ASSERT_EQ(wasMuted, ac->IsMuted());
}
