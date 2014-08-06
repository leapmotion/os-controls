set(utility_SOURCES
  utility.h
  utility.cpp
  ComInitializer.h
  ComInitializer.cpp
  Config.h
  Config.cpp
)

add_library(utility ${utility_SOURCES})
target_link_libraries(utility Autowiring)
target_include_directories(utility PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
