set(graphics_SOURCES
  ExposeActivationStateMachine.h
  ExposeActivationStateMachine.cpp
  Renderable.h
  Renderable.cpp
  RenderEngine.h
  RenderEngine.cpp
  RenderFrame.h
  VolumeKnob.cpp
  VolumeKnob.h
)


ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" graphics_SOURCES)

add_library(graphics ${graphics_EXEFLAGS} ${graphics_SOURCES})

target_link_libraries(graphics Autowiring)
target_include_directories(graphics PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

target_package(graphics SFML 2.1 REQUIRED)
target_package(graphics Leap REQUIRED)

target_link_components(graphics Animation Primitives SceneGraph Resource GLShader GLShaderLoader RadialMenu HandCursor)
