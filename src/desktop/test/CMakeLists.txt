set(desktoptest_SRCS
  VirtualScreenTest.cpp
  desktoptest.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" desktoptest_SRCS)
add_executable(desktoptest ${desktoptest_SRCS})
target_link_libraries(desktoptest desktop AutoTesting)
target_include_directories(desktoptest PUBLIC ..)
target_include_directories(desktoptest PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

# This is a unit test, let CMake know this
add_test(NAME desktoptest COMMAND $<TARGET_FILE:desktoptest>)
