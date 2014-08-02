set(oscontrol_SOURCES
  oscontrol.h
  oscontrol.cpp
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
  oscontrol_SOURCES
  MediaControllerWin.h
  MediaControllerWin.cpp
)

add_mac_sources(
  oscontrol_SOURCES
  MediaControllerMac.h
  MediaControllerMac.cpp
)

if(WIN32)
  set(oscontrol_EXEFLAGS WIN32 ${oscontrol_EXEFLAGS})
endif()

add_executable(oscontrol ${oscontrol_EXEFLAGS} ${oscontrol_SOURCES})
target_link_libraries(oscontrol Autowiring utility interaction engine)
target_package(oscontrol SDL 2.0.1 REQUIRED)
target_package(oscontrol Leap REQUIRED)
target_include_directories(oscontrol PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
