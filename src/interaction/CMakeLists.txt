set(interaction_SOURCES
  GestureTriggerManifest.cpp
  GestureTriggerManifest.h
  CoordinateUtility.cpp
  CoordinateUtility.h
  ClawRotationRecognizer.cpp
  ClawRotationRecognizer.h
  ExposeViewStateMachine.h
  ExposeViewStateMachine.cpp
  TimeRecognizer.h
  TimeRecognizer.cpp
  FrameFragmenter.h
  FrameFragmenter.cpp
  HandDataCombiner.h
  HandDataCombiner.cpp
  HandEventListener.h
  HandLocationRecognizer.cpp
  HandLocationRecognizer.h
  HandPinchRecognizer.h
  HandPinchRecognizer.cpp
  HandPoseRecognizer.cpp
  HandPoseRecognizer.h
  HandRollRecognizer.h
  HandRollRecognizer.cpp
  InteractionConfigs.h
  MediaViewController.h
  MediaViewController.cpp
  PinchScrollRecognizer.h
  PinchScrollRecognizer.cpp
  ScrollRecognizer.h
  ScrollRecognizer.cpp
  ScrollStateMachine.h
  ScrollStateMachine.cpp
  StateMachine.h
  StateMachine.cpp
  StateMachineContext.h
)

add_library(interaction ${interaction_EXEFLAGS} ${interaction_SOURCES})

target_link_libraries(interaction uievents Autowiring)
target_include_directories(interaction PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

target_package(interaction SFML 2.1 REQUIRED)
target_package(interaction Eigen 3.2.1 REQUIRED)
target_package(interaction Leap REQUIRED)

target_link_components(interaction EigenTypes Animation Primitives SceneGraph Resource GLShader GLShaderLoader RadialMenu HandCursor)
