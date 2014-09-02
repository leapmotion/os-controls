set(expose_SOURCES
  ExposeView.h
  ExposeView.cpp
  ExposeViewAccessManager.h
  ExposeViewAccessManager.cpp
  ExposeViewController.h
  ExposeViewController.cpp
  ExposeViewEvents.h
  ExposeViewWindow.h
  ExposeViewWindow.cpp
)


ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" expose_SOURCES)

add_library(expose ${expose_EXEFLAGS} ${expose_SOURCES})

target_link_libraries(expose Autowiring)
target_include_directories(expose PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

target_package(expose SFML 2.1 REQUIRED)
target_package(expose Leap REQUIRED)

target_link_components(expose Animation Primitives SceneGraph Resource GLShader GLShaderLoader)

add_subdirectory(testharness)