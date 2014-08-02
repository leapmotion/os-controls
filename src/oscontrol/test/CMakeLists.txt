set(oscontroltest_SRCS
  oscontroltest.cpp
  MediaControllerTest.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" oscontroltest_SRCS)
add_executable(oscontroltest ${oscontroltest_SRCS})
target_link_libraries(oscontroltest oscontrol AutoTesting)
target_include_directories(oscontroltest PUBLIC ..)
target_include_directories(oscontroltest PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

# This is a unit test, let CMake know this
add_test(NAME oscontroltest COMMAND $<TARGET_FILE:oscontroltest>)
