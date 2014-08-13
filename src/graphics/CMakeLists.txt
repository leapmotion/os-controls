set(graphics_SOURCES
  ExposeControl.h
  ExposeControl.cpp
  graphics.h
  graphics.cpp
  GraphicsStateMachine.h
  GraphicsStateMachine.cpp
  RenderEngine.h
  RenderEngine.cpp
  RenderFrame.h
  State.h
  VolumeControl.h
  VolumeControl.cpp
)


ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" graphics_SOURCES)

add_library(graphics ${graphics_EXEFLAGS} ${graphics_SOURCES})

target_link_libraries(graphics PUBLIC Autowiring)
target_include_directories(graphics PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

target_package(graphics SFML 2.1 REQUIRED LINK_TYPE PUBLIC)
target_package(graphics Leap REQUIRED LINK_TYPE PUBLIC)

target_link_components(graphics PUBLIC Primitives SceneGraph Resource GLShader GLShaderLoader)
