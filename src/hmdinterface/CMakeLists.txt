set(hmdinterface_SOURCES
  IContext.h
  IDevice.h
  IDevice.cpp #Remove me ASAP!
  IDeviceConfiguration.h
  IEyeConfiguration.h
  IException.h
  IntermediateArray.h
  IPose.h
  MatrixComponentOrder.h

  HmdFactory.h
  HmdFactory.cpp

  OculusRift/RiftContext.h
  OculusRift/RiftContext.cpp
  OculusRift/RiftDevice.h
  OculusRift/RiftDevice.cpp
  OculusRift/RiftDeviceConfiguration.cpp
  OculusRift/RiftDeviceConfiguration.h
  OculusRift/RiftException.h
  OculusRift/RiftEyeConfiguration.h
  OculusRift/RiftEyeConfiguration.cpp
  OculusRift/RiftPose.h
  OculusRift/RiftPose.cpp
)

add_library(hmdinterface ${hmdinterface_SOURCES})

set_property(TARGET hmdinterface PROPERTY FOLDER "Common")

target_package(hmdinterface OculusSDK 0.4.1 REQUIRED LINK_TYPE PUBLIC)

target_link_libraries(hmdinterface PUBLIC C++11 LeapGLCore)

# For ContextMember static new support ONLY - 
# when non-inheriting factories are enabled, this can be removed

target_link_libraries(hmdinterface PUBLIC Autowiring)

if(APPLE)
  find_library(IOKIT_FRAMEWORK IOKit)
  target_link_libraries(hmdinterface PUBLIC ${IOKIT_FRAMEWORK})
endif()
target_include_directories(hmdinterface PUBLIC .)

add_subdirectory(test)