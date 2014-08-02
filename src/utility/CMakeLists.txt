set(utility_SOURCES
  utility.h
  utility.cpp
  Config.h
  Config.cpp
)

add_windows_sources(
  utility_SOURCES
  ComInitializer.h
  ComInitializer.cpp
)

add_library(utility ${utility_SOURCES})
target_link_libraries(utility Autowiring)
target_include_directories(utility PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
