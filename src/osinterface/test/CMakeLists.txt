set(osinterfacetest_SRCS
  osinterfacetest.cpp
  AudioVolumeInterfaceTest.cpp
  MediaInterfaceTest.cpp
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

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" osinterfacetest_SRCS)
add_executable(osinterfacetest ${osinterfacetest_SRCS})
target_link_libraries(osinterfacetest osinterface AutoTesting)
target_include_directories(osinterfacetest PUBLIC ..)
target_include_directories(osinterfacetest PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

if(WIN32)
  set_property(TARGET osinterfacetest APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:libcmt.lib")
endif()

# This is a unit test, let CMake know this
add_test(NAME osinterfacetest COMMAND $<TARGET_FILE:osinterfacetest>)
