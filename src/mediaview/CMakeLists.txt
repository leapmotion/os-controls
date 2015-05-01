set(mediaview_SOURCES
  MediaViewController.h
  MediaViewController.cpp
  MediaViewStateMachine.h
  MediaViewStateMachine.cpp
  VolumeSliderView.h
  VolumeSliderView.cpp
)

add_pch(graphics_SOURCES "stdafx.h" "stdafx.cpp")

add_library(mediaview ${mediaview_EXEFLAGS} ${mediaview_SOURCES})

set_property(TARGET mediaview PROPERTY FOLDER "Shortcuts")

target_link_libraries(mediaview Autowiring cursor graphics RadialMenu)

