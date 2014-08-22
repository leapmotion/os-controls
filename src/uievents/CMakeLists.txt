set(uievents_SOURCES
  AbstractVolumeControl.h
  AudioVolumeInterface.h
  AudioVolumeInterface.cpp
  osControlConfigs.h
  OSCDomain.h
  MediaViewEventListener.h
  MediaViewEventListener.cpp
  SystemMultimediaEventListener.h
  Updatable.h
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" uievents_SOURCES)
add_library(uievents ${uievents_SOURCES})
