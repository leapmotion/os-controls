
set(utility_SOURCES
  CircleFitter.h
  Config.h
  Config.cpp
  ConfigEvent.h
  ExtendedStateMachine.h
  FileMonitor.h
  HandleUtilitiesWin.h
  Hysteresis.h
  Hysteresis.cpp
  HysteresisRange.h
  HysteresisRange.cpp
  HysteresisVolume.h
  lockable_property.h
  NativeWindow.h
  PlatformInitializer.h
  SamplePrimitives.h
  SamplePrimitives.cpp
)

add_windows_sources(utility_SOURCES
  FileMonitorWin.h
  FileMonitorWin.cpp
  NativeWindowWin.cpp
  PlatformInitializerWin.h
  PlatformInitializerWin.cpp
)

add_mac_sources(utility_SOURCES
  FileMonitorMac.h
  FileMonitorMac.cpp
  NativeWindowMac.mm
  PlatformInitializerMac.h
  PlatformInitializerMac.mm
)

add_library(utility ${utility_SOURCES})
target_link_libraries(utility Autowiring json11 )

if(MSVC)
  target_link_libraries(utility shlwapi)
endif()

target_link_components(utility Primitives)
add_subdirectory(test)
