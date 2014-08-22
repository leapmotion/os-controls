set(graphics_SOURCES
  Cursor.h
  Cursor.cpp
  ExposeControl.h
  ExposeControl.cpp
  MediaView.h
  MediaView.cpp
  MediaViewEventListener.h
  RadialButton.h
  RadialButton.cpp
  Renderable.h
  RenderEngine.h
  RenderEngine.cpp
  RenderEngineNode.h
  RenderFrame.h
  Updatable.h
  VolumeControl.h
  VolumeControl.cpp
  MostRecent.h
  GraphicsConfigs.h
)


ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" graphics_SOURCES)

add_library(graphics ${graphics_EXEFLAGS} ${graphics_SOURCES})

target_link_libraries(graphics Autowiring)
target_include_directories(graphics PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

target_package(graphics SFML 2.1 REQUIRED)
target_package(graphics Leap REQUIRED)

target_link_components(graphics Animation Primitives SceneGraph Resource GLShader GLShaderLoader)
