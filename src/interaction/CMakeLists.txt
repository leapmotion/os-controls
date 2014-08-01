set(interaction_SOURCES
  interaction.h
  interaction.cpp
)

add_library(interaction ${interaction_EXEFLAGS} ${interaction_SOURCES})
target_link_libraries(interaction Autowiring)
target_package(interaction SDL 2.0.1 REQUIRED)
target_package(interaction Leap REQUIRED)
target_include_directories(interaction PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
