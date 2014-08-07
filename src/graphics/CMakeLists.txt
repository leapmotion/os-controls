set(graphics_SOURCES
  graphics.h
  graphics.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" graphics_SOURCES)
add_library(graphics ${graphics_EXEFLAGS} ${graphics_SOURCES})
target_link_libraries(graphics Autowiring)
target_package(graphics SDL 2.0.1 REQUIRED)
target_package(graphics Leap REQUIRED)
target_include_directories(graphics PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
