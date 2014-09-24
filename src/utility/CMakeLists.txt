
set(utility_SOURCES
  AutoLaunch.h
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
  AutoLaunchWin.h
  AutoLaunchWin.cpp
  FileMonitorWin.h
  FileMonitorWin.cpp
  NativeWindowWin.cpp
  PlatformInitializerWin.h
  PlatformInitializerWin.cpp
  SingleInstanceEnforcerWin.h
  SingleInstanceEnforcerWin.cpp
)

add_mac_sources(utility_SOURCES
  AutoLaunchMac.h
  AutoLaunchMac.mm
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
elseif(APPLE)
  find_library(CORESERVICES_FRAMEWORK CoreServices)
  mark_as_advanced(CORESERVICES_FRAMEWORK)
  target_link_libraries(utility ${CORESERVICES_FRAMEWORK})
endif()

target_link_components(utility Primitives)
add_subdirectory(test)
