set(utilitytest_SRCS
  utilitytest.cpp
  ConfigTest.cpp
  LockablePropertyTest.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" utilitytest_SRCS)
add_executable(utilitytest ${utilitytest_SRCS})
target_link_libraries(utilitytest utility AutoTesting)
target_include_directories(utilitytest PUBLIC ..)
target_include_directories(utilitytest PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

# This is a unit test, let CMake know this
add_test(NAME utilitytest COMMAND $<TARGET_FILE:utilitytest>)
