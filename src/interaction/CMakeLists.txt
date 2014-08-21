set(interaction_SOURCES
  CircleGestureTrigger.cpp
  CircleGestureTrigger.h
  HandExistTrigger.cpp
  HandExistTrigger.h
  GestureTriggerManifest.cpp
  GestureTriggerManifest.h
  CoordinateUtility.cpp
  CoordinateUtility.h
  FrameFragmenter.h
  FrameFragmenter.cpp
  HandCoordinatesDecorator.cpp
  HandCoordinatesDecorator.h
  HandPoseDecorator.cpp
  HandPoseDecorator.h
  InteractionConfigs.h
  StateMachine.h
  StateMachine.cpp
  StateMachineContext.h
)

add_library(interaction ${interaction_EXEFLAGS} ${interaction_SOURCES})

target_link_libraries(interaction Autowiring)
target_include_directories(interaction PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

target_package(interaction SFML 2.1 REQUIRED)
target_package(interaction Eigen 3.2.1 REQUIRED)
target_package(interaction Leap REQUIRED)

target_link_components(interaction EigenTypes)

