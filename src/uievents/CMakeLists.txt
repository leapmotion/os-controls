set(uievents_SOURCES
  ShortcutsConfigs.h
  ShortcutsDomain.h
  MediaViewEventListener.h
  MediaViewEventListener.cpp
  SystemMultimediaEventListener.h
  Updatable.h
)

add_pch(uievents_SOURCES "stdafx.h" "stdafx.cpp")
add_library(uievents ${uievents_SOURCES})
set_property(TARGET uievents PROPERTY FOLDER "Shortcuts")

target_link_libraries(uievents Autowiring)
