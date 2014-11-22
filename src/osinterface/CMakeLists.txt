set(osinterface_SOURCES
  AudioVolumeInterface.h
  AudioVolumeInterface.cpp
  CompositionEngine.h
  CompositionEngine.cpp
  HtmlPageLauncher.h
  HtmlPageLauncher.cpp
  KeepRenderWindowFullScreen.h
  KeepRenderWindowFullScreen.cpp
  LeapInput.h
  LeapInput.cpp
  LeapInputListener.h
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
  OSKeyboardEvent.h
  OSScreen.h
  OSScreenBase.h
  OSVirtualScreen.h
  OSVirtualScreen.cpp
  OSWindow.h
  OSWindow.cpp
  OSWindowEvent.h
  OSWindowHandle.h
  OSWindowMonitor.h
  OSWindowMonitor.cpp
  RenderContext.h
  RenderWindow.h
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
  RenderContextWin.h
  RenderContextWin.cpp
  RenderWindowWin.h
  RenderWindowWin.cpp
  WindowScrollerWin.h
  WindowScrollerWin.cpp
)

add_mac_sources(
  osinterface_SOURCES
  AudioVolumeInterfaceMac.h
  AudioVolumeInterfaceMac.cpp
  CompositionEngineMac.h
  CompositionEngineMac.cpp
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
  RenderContextMac.h
  RenderContextMac.mm
  RenderWindowMac.h
  RenderWindowMac.mm
  WindowScrollerMac.h
  WindowScrollerMac.mm
)

add_library(osinterface ${osinterface_SOURCES})
set_property(TARGET osinterface PROPERTY FOLDER "Common")

target_link_libraries(osinterface PUBLIC Autowiring utility)
target_package(osinterface Leap REQUIRED LINK_TYPE PUBLIC)
target_package(osinterface Glew REQUIRED LINK_TYPE PUBLIC)

target_link_libraries(osinterface PUBLIC Primitives Animation)

if(APPLE)
  find_library(FOUNDATION_FRAMEWORK Foundation)
  find_library(APPKIT_FRAMEWORK AppKit)
  find_library(CORE_AUDIO_FRAMEWORK CoreAudio)
  mark_as_advanced(FOUNDATION_FRAMEWORK)
  mark_as_advanced(APPKIT_FRAMEWORK)
  mark_as_advanced(CORE_AUDIO_FRAMEWORK)
  target_link_libraries(osinterface PUBLIC ${CORE_AUDIO_FRAMEWORK} ${APPKIT_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
endif()

if(MSVC)
  target_package(osinterface FreeImage REQUIRED LINK_TYPE PUBLIC)
  set_property(TARGET osinterface APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:libcmt.lib")
  target_link_libraries(osinterface PUBLIC opengl32 dwmapi psapi)
  target_link_libraries(osinterface PUBLIC d3d11 dcomp) #used by CompositionEngineWin
endif()

add_subdirectory(test)
