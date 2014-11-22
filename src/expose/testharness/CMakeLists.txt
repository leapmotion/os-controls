set(expose_testharness_SOURCES
  ExposeTestHarness.h
  ExposeTestHarness.cpp
  TestStateMachine.h
  TestStateMachine.cpp
)

if(WIN32)
  set(expose_testharness_EXEFLAGS WIN32)
elseif(APPLE)
  set_property(SOURCE Shortcuts.cpp PROPERTY COMPILE_FLAGS "-x objective-c++")
  set(expose_testharness_EXEFLAGS MACOSX_BUNDLE)
endif()
add_pch(expose_testharness_SOURCES "stdafx.h" "stdafx.cpp")

add_executable(expose_testharness ${expose_testharness_EXEFLAGS} ${expose_testharness_SOURCES})
set_property(TARGET expose_testharness PROPERTY FOLDER "Shortcuts")

target_link_libraries(expose_testharness expose utility osinterface graphics)
target_include_directories(expose_testharness PRIVATE "..")

target_package(expose_testharness Leap REQUIRED)
target_package(expose_testharness SFML 2.1 REQUIRED)

if(WIN32)
  set_property(TARGET expose_testharness APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:libcmt.lib")
endif()