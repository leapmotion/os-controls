set(osinterface_SOURCES
  AudioVolumeController.h
  AudioVolumeController.cpp
  LeapInput.h
  LeapInput.cpp
  MainWindow.h
  MainWindow.cpp
  MediaController.h
  MediaController.cpp
  SdlInitializer.h
  SdlInitializer.cpp
  RenderingContext.h
  RenderingContext.cpp
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
  MediaControllerMac.h
  MediaControllerMac.cpp
)

add_library(osinterface ${osinterface_SOURCES})
target_link_libraries(osinterface Autowiring utility)
target_package(osinterface SDL 2.0.1 REQUIRED)
target_package(osinterface Leap REQUIRED)
target_include_directories(osinterface PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

add_subdirectory(test)
