set(interaction_SOURCES
  HandExistTrigger.cpp
  HandExistTrigger.h
  GestureTriggerManifest.cpp
  GestureTriggerManifest.h
)

add_library(interaction ${interaction_EXEFLAGS} ${interaction_SOURCES})
target_link_libraries(interaction Autowiring)
target_package(interaction SFML 2.1 REQUIRED)
target_package(interaction Leap REQUIRED)
target_include_directories(interaction PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
