set(osinterface_SOURCES
  AudioVolumeInterface.h
  AudioVolumeInterface.cpp
  LeapInput.h
  LeapInput.cpp
  LeapInputListener.h
  MakesRenderWindowFullScreen.h
  MakesRenderWindowFullScreen.cpp
  MediaInterface.h
  MediaInterface.cpp
  VolumeLevelChecker.h
  VolumeLevelChecker.cpp
  OSGeometry.h
  OSScreen.h
  OSScreenBase.h
  OSVirtualScreen.h
  OSVirtualScreen.cpp
  OSWindow.h
  OSWindow.cpp
  OSWindowEvent.h
  OSWindowEvent.cpp
  OSWindowMonitor.h
  OSWindowMonitor.cpp
  OSWindowNode.h
  ScrollOperation.h
  WindowScroller.h
  WindowScroller.cpp
)

add_windows_sources(
  osinterface_SOURCES
  AudioVolumeInterfaceWin.h
  AudioVolumeInterfaceWin.cpp
  MediaInterfaceWin.h
  MediaInterfaceWin.cpp
  OSScreenWin.cpp
  OSVirtualScreenWin.h
  OSVirtualScreenWin.cpp
  OSWindowWin.h
  OSWindowWin.cpp
  OSWindowMonitorWin.h
  OSWindowMonitorWin.cpp
  WindowScrollerWin.h
  WindowScrollerWin.cpp
)

add_mac_sources(
  osinterface_SOURCES
  AudioVolumeInterfaceMac.h
  AudioVolumeInterfaceMac.cpp
  MediaInterfaceMac.h
  MediaInterfaceMac.mm
  OSScreenMac.mm
  OSVirtualScreenMac.h
  OSVirtualScreenMac.mm
  #  OSWindowMac.h
  #  OSWindowMac.mm
  OSWindowMonitorMac.h
  OSWindowMonitorMac.mm
  WindowScrollerMac.h
  WindowScrollerMac.mm
)

add_library(osinterface ${osinterface_SOURCES})
target_link_libraries(osinterface Autowiring utility)
target_package(osinterface SFML 2.1 REQUIRED)
target_package(osinterface Leap REQUIRED)
target_include_directories(osinterface PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

target_link_components(osinterface GLTexture2)

if(APPLE)
  find_library(FOUNDATION_FRAMEWORK Foundation)
  find_library(APPKIT_FRAMEWORK AppKit)
  find_library(CORE_AUDIO_FRAMEWORK CoreAudio)
  mark_as_advanced(FOUNDATION_FRAMEWORK)
  mark_as_advanced(APPKIT_FRAMEWORK)
  mark_as_advanced(CORE_AUDIO_FRAMEWORK)
  target_link_libraries(osinterface ${CORE_AUDIO_FRAMEWORK} ${APPKIT_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
endif()

add_subdirectory(test)
