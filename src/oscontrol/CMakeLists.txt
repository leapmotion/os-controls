set(oscontrol_SOURCES
  oscontrol.h
  oscontrol.cpp
  MainWindow.h
  MainWindow.cpp
)

add_executable(oscontrol ${oscontrol_SOURCES})
target_package(oscontrol SDL 2.0.1 REQUIRED)
target_include_directories(oscontrol PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)