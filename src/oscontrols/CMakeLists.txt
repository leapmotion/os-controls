set(oscontrols_SOURCES
  oscontrols.h
  oscontrols.cpp
)

if(WIN32)
  set(oscontrols_EXEFLAGS WIN32)
elseif(APPLE)
  set_property(SOURCE oscontrols.cpp PROPERTY COMPILE_FLAGS "-x objective-c++")
  set(oscontrols_EXEFLAGS MACOSX_BUNDLE)
  set(MACOSX_BUNDLE_BUNDLE_NAME "oscontrols")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "1")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "0.0.1")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "0.0.1")
  set(MACOSX_BUNDLE_COPYRIGHT "Copyright © 2014 Leap Motion, Inc. All rights reserved.")
endif()

add_executable(oscontrols ${oscontrols_EXEFLAGS} ${oscontrols_SOURCES})
target_link_libraries(oscontrols Autowiring utility interaction graphics osinterface)
target_include_directories(oscontrols PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

target_package(oscontrols Leap REQUIRED )
target_package(oscontrols SFML 2.1 REQUIRED )

