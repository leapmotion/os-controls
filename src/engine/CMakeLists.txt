set(engine_SOURCES
  engine.h
  engine.cpp
)

add_library(engine ${engine_EXEFLAGS} ${engine_SOURCES})
target_link_libraries(engine Autowiring)
target_package(engine SDL 2.0.1 REQUIRED)
target_package(engine Leap REQUIRED)
target_include_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
