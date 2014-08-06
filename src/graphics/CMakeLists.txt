set(graphics_SOURCES
  graphics.h
  graphics.cpp
)

add_library(graphics ${graphics_EXEFLAGS} ${graphics_SOURCES})
target_link_libraries(graphics Autowiring)
target_package(graphics SFML 2.1 REQUIRED)
target_package(graphics Leap REQUIRED)
target_include_directories(graphics PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
