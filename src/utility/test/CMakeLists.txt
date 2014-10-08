set(utilitytest_SRCS
  utilitytest.cpp
  ConfigTest.cpp
  FileMonitorTest.h
  FileMonitorTest.cpp
  HysteresisTest.cpp
  LockablePropertyTest.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" utilitytest_SRCS)
add_executable(utilitytest ${utilitytest_SRCS})
set_property(TARGET utilitytest PROPERTY FOLDER "Tests")

find_package(Boost REQUIRED COMPONENTS filesystem)
target_link_libraries(utilitytest ${Boost_LIBRARIES})
target_include_directories(utilitytest PUBLIC ${Boost_INCLUDE_DIR})

target_link_libraries(utilitytest utility AutoTesting)
target_include_directories(utilitytest PUBLIC ..)

# This is a unit test, let CMake know this
add_test(NAME utilitytest COMMAND $<TARGET_FILE:utilitytest>)
