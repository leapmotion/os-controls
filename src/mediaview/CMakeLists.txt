set(mediaview_SOURCES
  MediaViewController.h
  MediaViewController.cpp
  MediaViewStateMachine.h
  MediaViewStateMachine.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" graphics_SOURCES)

add_library(mediaview ${mediaview_EXEFLAGS} ${mediaview_SOURCES})

target_link_libraries(mediaview Autowiring graphics)
target_include_directories(mediaview PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

