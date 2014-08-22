set(utility_SOURCES
  Config.h
  Config.cpp
  ExtendedStateMachine.h
  Geometry.h
  NativeWindow.h
  PlatformInitializer.h
)

add_windows_sources(utility_SOURCES 
  NativeWindowWin.cpp
  PlatformInitializerWin.h
  PlatformInitializerWin.cpp
)

add_mac_sources(utility_SOURCES
  NativeWindowMac.cpp
  PlatformInitializerMac.h
  PlatformInitializerMac.cpp
)
if(APPLE)
  set_property(SOURCE NativeWindowMac.cpp PlatformInitializerMac.cpp PROPERTY COMPILE_FLAGS "-x objective-c++")
endif()

add_library(utility ${utility_SOURCES})
target_link_libraries(utility Autowiring)
target_include_directories(utility PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
