set(graphics_SOURCES
  Renderable.h
  Renderable.cpp
  RenderEngine.h
  RenderEngine.cpp
  RenderFrame.h
)

add_pch(graphics_SOURCES "stdafx.h" "stdafx.cpp")

add_library(graphics ${graphics_SOURCES})

set_property(TARGET graphics PROPERTY FOLDER "Common")

target_link_libraries(graphics Autowiring)

target_package(graphics Leap REQUIRED)

target_link_libraries(graphics Animation Primitives Resource GLShaderLoader)
