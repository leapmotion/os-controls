set(uievents_SOURCES
  ShortcutsConfigs.h
  ShortcutsDomain.h
  MediaViewEventListener.h
  MediaViewEventListener.cpp
  SystemMultimediaEventListener.h
  Updatable.h
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" uievents_SOURCES)
add_library(uievents ${uievents_SOURCES})
set_property(TARGET uievents PROPERTY FOLDER "Shortcuts")

target_link_libraries(uievents Autowiring)
