set(osinterfacetest_SRCS
  osinterfacetest.cpp
  AudioVolumeInterfaceTest.cpp
  CompositionEngineTest.cpp
  MediaInterfaceTest.cpp
  HtmlPageLauncherTest.cpp
  OSCursorTest.cpp
  OSWindowInterfaceTest.cpp
  OSVirtualScreenTest.cpp
  WindowCreatingTestFixture.h
  WindowCreatingTestFixture.cpp
  WindowScrollerTest.cpp
)

add_windows_sources(
  osinterfacetest_SRCS
  WindowCreatingTestFixtureWin.h
  WindowCreatingTestFixtureWin.cpp
)

add_mac_sources(
  osinterfacetest_SRCS
  WindowCreatingTestFixtureMac.h
  WindowCreatingTestFixtureMac.cpp
)

add_pch(osinterfacetest_SRCS "stdafx.h" "stdafx.cpp")
add_executable(osinterfacetest ${osinterfacetest_SRCS})
set_property(TARGET osinterfacetest PROPERTY FOLDER "Tests")

target_link_libraries(osinterfacetest osinterface AutoTesting)
target_include_directories(osinterfacetest PUBLIC ..)

if(WIN32)
  set_property(TARGET osinterfacetest APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:libcmt.lib")
endif()

# This is a unit test, let CMake know this
add_test(NAME osinterfacetest COMMAND $<TARGET_FILE:osinterfacetest>)
