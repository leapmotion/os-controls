set(graphics_SOURCES
  Renderable.h
  Renderable.cpp
  RenderEngine.h
  RenderEngine.cpp
  RenderFrame.h
)


ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" graphics_SOURCES)

add_library(graphics ${graphics_EXEFLAGS} ${graphics_SOURCES})

target_link_libraries(graphics Autowiring)

target_package(graphics SFML 2.1 REQUIRED)
target_package(graphics Leap REQUIRED)

target_link_libraries(graphics Animation Primitives SceneGraph Resource GLShader GLShaderLoader RadialMenu HandCursor)
