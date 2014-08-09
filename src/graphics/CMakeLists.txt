set(graphics_SOURCES
  ExposeControl.h
  ExposeControl.cpp
  graphics.h
  graphics.cpp
  GraphicsStateMachine.h
  GraphicsStateMachine.cpp
  State.h
  VolumeControl.h
  VolumeControl.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" graphics_SOURCES)
add_library(graphics ${graphics_EXEFLAGS} ${graphics_SOURCES})
target_link_libraries(graphics Autowiring)
target_package(graphics SFML 2.1 REQUIRED)
target_package(graphics Leap REQUIRED)
target_include_directories(graphics PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
