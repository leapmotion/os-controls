set(osinterface_SOURCES
  Screen.h
  ScreenBase.h
  VirtualScreen.h
  VirtualScreen.cpp
)

add_windows_sources(
  osinterface_SOURCES
  ScreenWin.cpp
  VirtualScreenWin.h
  VirtualScreenWin.cpp
)

add_mac_sources(
  osinterface_SOURCES
  ScreenMac.mm
  VirtualScreenMac.h
  VirtualScreenMac.mm
)

add_library(desktop ${osinterface_SOURCES})
target_link_libraries(desktop utility)
target_include_directories(desktop PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
if(APPLE)
  find_library(FOUNDATION_FRAMEWORK Foundation)
  find_library(APPKIT_FRAMEWORK AppKit)
  mark_as_advanced(FOUNDATION_FRAMEWORK)
  mark_as_advanced(APPKIT_FRAMEWORK)
  target_link_libraries(desktop ${APPKIT_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
endif()

add_subdirectory(test)
