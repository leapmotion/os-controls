set(hmdinterfacetest_SRCS
  hmdinterfacetest.cpp
  OculusTest.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" hmdinterfacetest_SRCS)
add_executable(hmdinterfacetest ${hmdinterfacetest_SRCS})
set_property(TARGET hmdinterfacetest PROPERTY FOLDER "Tests")

if(WIN32)
  set_property(TARGET hmdinterfacetest APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:libcmt.lib")
endif()

target_link_libraries(hmdinterfacetest hmdinterface SDLController AutoTesting)
target_include_directories(hmdinterfacetest PUBLIC ..)

# This is a unit test, let CMake know this
add_test(NAME hmdinterfacetest COMMAND $<TARGET_FILE:hmdinterfacetest>)
