set(expose_SOURCES
  ExposeActivationStateMachine.h
  ExposeActivationStateMachine.cpp
  ExposeGroup.h
  ExposeGroup.cpp
  ExposeView.h
  ExposeView.cpp
  ExposeViewAccessManager.h
  ExposeViewAccessManager.cpp
  ExposeViewController.h
  ExposeViewController.cpp
  ExposeViewEvents.h
  ExposeViewEvents.cpp
  ExposeViewStateMachine.h
  ExposeViewStateMachine.cpp
  ExposeViewWindow.h
  ExposeViewWindow.cpp
)


ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" expose_SOURCES)

add_library(expose ${expose_EXEFLAGS} ${expose_SOURCES})
set_property(TARGET expose PROPERTY FOLDER "Shortcuts")
target_link_libraries(expose Autowiring utility)

target_package(expose Leap REQUIRED)

target_link_libraries(expose Animation Primitives SceneGraph RadialMenu Resource GLShader GLShaderLoader HandCursor)

add_subdirectory(testharness)
