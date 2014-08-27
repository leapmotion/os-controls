set(interaction_SOURCES
  GestureTriggerManifest.cpp
  GestureTriggerManifest.h
  CoordinateUtility.cpp
  CoordinateUtility.h
  ExposeViewAccessManager.h
  ExposeViewAccessManager.cpp
  ExposeViewProxy.h
  ExposeViewProxy.cpp
  FrameFragmenter.h
  FrameFragmenter.cpp
  HandEventListener.h
  HandLocationRecognizer.cpp
  HandLocationRecognizer.h
  HandPoseRecognizer.cpp
  HandPoseRecognizer.h
  InteractionConfigs.h
  MediaViewController.h
  MediaViewController.cpp
  HandRollRecognizer.h
  HandRollRecognizer.cpp
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

target_link_components(interaction EigenTypes Animation Primitives SceneGraph Resource GLShader GLShaderLoader)
