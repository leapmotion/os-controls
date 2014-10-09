set(osinterface_SOURCES
  AudioVolumeInterface.h
  AudioVolumeInterface.cpp
  CompositionEngine.h
  CompositionEngine.cpp
  HtmlPageLauncher.h
  HtmlPageLauncher.cpp
  LeapInput.h
  LeapInput.cpp
  LeapInputListener.h
  MakesRenderWindowFullScreen.h
  MakesRenderWindowFullScreen.cpp
  MediaInterface.h
  MediaInterface.cpp
  VolumeLevelChecker.h
  VolumeLevelChecker.cpp
  OSApp.h
  OSAppManager.h
  OSAppManager.cpp
  OSGeometry.h
  OSCursor.h
  OSCursor.cpp
  OSScreen.h
  OSScreenBase.h
  OSVirtualScreen.h
  OSVirtualScreen.cpp
  OSWindow.h
  OSWindow.cpp
  OSWindowEvent.h
  OSWindowMonitor.h
  OSWindowMonitor.cpp
  ScrollOperation.h
  WindowScroller.h
  WindowScroller.cpp
)

add_windows_sources(
  osinterface_SOURCES
  AudioVolumeInterfaceWin.h
  AudioVolumeInterfaceWin.cpp
  CompositionEngineWin.h
  CompositionEngineWin.cpp
  HtmlPageLauncherWin.h
  HtmlPageLauncherWin.cpp
  MediaInterfaceWin.h
  MediaInterfaceWin.cpp
  OSAppWin.h
  OSAppWin.cpp
  OSCursorWin.h
  OSCursorWin.cpp
  OSScreenWin.cpp
  OSWindowWin.h
  OSWindowWin.cpp
  OSWindowMonitorWin.h
  OSWindowMonitorWin.cpp
  OSVirtualScreenWin.h
  OSVirtualScreenWin.cpp
  WindowScrollerWin.h
  WindowScrollerWin.cpp
)

add_mac_sources(
  osinterface_SOURCES
  AudioVolumeInterfaceMac.h
  AudioVolumeInterfaceMac.cpp
  HtmlPageLauncherMac.h
  HtmlPageLauncherMac.mm
  MediaInterfaceMac.h
  MediaInterfaceMac.mm
  OSAppMac.h
  OSAppMac.mm
  OSCursorMac.h
  OSCursorMac.cpp
  OSScreenMac.mm
  OSVirtualScreenMac.h
  OSVirtualScreenMac.mm
  OSWindowMac.h
  OSWindowMac.mm
  OSWindowMonitorMac.h
  OSWindowMonitorMac.mm
  WindowScrollerMac.h
  WindowScrollerMac.mm
)

add_library(osinterface ${osinterface_SOURCES})
set_property(TARGET osinterface PROPERTY FOLDER "Common")

target_link_libraries(osinterface Autowiring utility)
target_package(osinterface SFML 2.1 REQUIRED)
target_package(osinterface Leap REQUIRED)

target_link_libraries(osinterface Primitives Animation)

if(APPLE)
  find_library(FOUNDATION_FRAMEWORK Foundation)
  find_library(APPKIT_FRAMEWORK AppKit)
  find_library(CORE_AUDIO_FRAMEWORK CoreAudio)
  mark_as_advanced(FOUNDATION_FRAMEWORK)
  mark_as_advanced(APPKIT_FRAMEWORK)
  mark_as_advanced(CORE_AUDIO_FRAMEWORK)
  target_link_libraries(osinterface ${CORE_AUDIO_FRAMEWORK} ${APPKIT_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
endif()

if(MSVC)
  set_property(TARGET osinterface APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:libcmt.lib")
endif()

add_subdirectory(test)
