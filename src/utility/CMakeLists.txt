set(utility_SOURCES
  Config.h
  Config.cpp
  ExtendedStateMachine.h
  lockable_property.h
  NativeWindow.h
  PlatformInitializer.h
)

add_windows_sources(utility_SOURCES 
  NativeWindowWin.cpp
  PlatformInitializerWin.h
  PlatformInitializerWin.cpp
)

add_mac_sources(utility_SOURCES
  NativeWindowMac.mm
  PlatformInitializerMac.h
  PlatformInitializerMac.mm
)

add_library(utility ${utility_SOURCES})
target_link_libraries(utility Autowiring)
target_include_directories(utility PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
add_subdirectory(test)
