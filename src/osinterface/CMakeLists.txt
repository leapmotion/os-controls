set(osinterface_SOURCES
  AudioVolumeController.h
  AudioVolumeController.cpp
  LeapInput.h
  LeapInput.cpp
  MediaController.h
  MediaController.cpp
  WindowingSystem.h
  WindowingSystem.cpp
  WindowSpaces.h
  WindowSpaces.cpp
)

add_windows_sources(
  osinterface_SOURCES
  AudioVolumeControllerWin.h
  AudioVolumeControllerWin.cpp
  MediaControllerWin.h
  MediaControllerWin.cpp
)

add_mac_sources(
  osinterface_SOURCES
  AudioVolumeControllerMac.h
  AudioVolumeControllerMac.cpp
  MediaControllerMac.h
  MediaControllerMac.cpp
)

add_library(osinterface ${osinterface_SOURCES})
target_link_libraries(osinterface Autowiring utility)
target_package(osinterface SFML 2.1 REQUIRED)
target_package(osinterface Leap REQUIRED)
target_include_directories(osinterface PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
if(APPLE)
  find_library(CORE_AUDIO_FRAMEWORK CoreAudio)
  include_directories(${CORE_AUDIO_FRAMEWORK}/Headers)
  mark_as_advanced(CORE_AUDIO_FRAMEWORK)
  target_link_libraries(osinterface ${CORE_AUDIO_FRAMEWORK})
endif()

add_subdirectory(test)
