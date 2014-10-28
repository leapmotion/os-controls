set(mediaview_SOURCES
  MediaViewController.h
  MediaViewController.cpp
  MediaViewStateMachine.h
  MediaViewStateMachine.cpp
  VolumeSliderView.h
  VolumeSliderView.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" graphics_SOURCES)

add_library(mediaview ${mediaview_EXEFLAGS} ${mediaview_SOURCES})

set_property(TARGET mediaview PROPERTY FOLDER "Shortcuts Projects")

target_link_libraries(mediaview Autowiring cursor graphics RadialMenu)

