set(interaction_SOURCES
  ClawRotationRecognizer.cpp
  ClawRotationRecognizer.h
  CoordinateUtility.cpp
  CoordinateUtility.h
  FrameFragmenter.cpp
  FrameFragmenter.h
  HandDataCombiner.cpp
  HandDataCombiner.h
  HandEventListener.h
  HandLocationRecognizer.cpp
  HandLocationRecognizer.h
  HandActivationRecognizer.cpp
  HandActivationRecognizer.h
  HandPoseRecognizer.cpp
  HandPoseRecognizer.h
  HandRollRecognizer.cpp
  HandRollRecognizer.h
  InteractionConfigs.h
  ScrollRecognizer.cpp
  ScrollRecognizer.h
  StateMachine.cpp
  StateMachine.h
  StateMachineContextManifest.cpp
  StateMachineContext.h
  StateMachineContextManifest.h
  SystemWipeRecognizer.cpp
  SystemWipeRecognizer.h
  TimeRecognizer.cpp
  TimeRecognizer.h
)

add_library(interaction ${interaction_EXEFLAGS} ${interaction_SOURCES})

set_property(TARGET interaction PROPERTY FOLDER "Common")

target_link_libraries(interaction Autowiring cursor expose mediaview uievents)

target_package(interaction Eigen 3.2.1 REQUIRED)
target_package(interaction Leap REQUIRED)

target_link_libraries(interaction EigenTypes Animation Primitives SceneGraph Resource GLShader GLShaderLoader RadialMenu HandCursor)

add_subdirectory(test)