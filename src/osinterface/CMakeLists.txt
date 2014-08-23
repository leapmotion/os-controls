set(osinterface_SOURCES
  AudioVolumeInterface.h
  AudioVolumeInterface.cpp
  LeapInput.h
  LeapInput.cpp
  MediaInterface.h
  MediaInterface.cpp
  VolumeLevelChecker.h
  VolumeLevelChecker.cpp
  WindowingSystem.h
  WindowingSystem.cpp
  WindowSpaces.h
  WindowSpaces.cpp
)

add_windows_sources(
  osinterface_SOURCES
  AudioVolumeInterfaceWin.h
  AudioVolumeInterfaceWin.cpp
  MediaInterfaceWin.h
  MediaInterfaceWin.cpp
)

add_mac_sources(
  osinterface_SOURCES
  AudioVolumeInterfaceMac.h
  AudioVolumeInterfaceMac.cpp
  MediaInterfaceMac.h
  MediaInterfaceMac.mm
)

add_library(osinterface ${osinterface_SOURCES})
target_link_libraries(osinterface Autowiring utility)
target_package(osinterface SFML 2.1 REQUIRED)
target_package(osinterface Leap REQUIRED)
target_include_directories(osinterface PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
if(APPLE)
  find_library(CORE_AUDIO_FRAMEWORK CoreAudio)
  mark_as_advanced(CORE_AUDIO_FRAMEWORK)
  target_link_libraries(osinterface ${CORE_AUDIO_FRAMEWORK})
endif()

add_subdirectory(test)
