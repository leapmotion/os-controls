set(VRShell_SOURCES
  LeapImagePassthrough.cpp
  LeapImagePassthrough.h
  VRShell.h
  VRShell.cpp
)

set(VRShell_RESOURCES
  ${CMAKE_SOURCE_DIR}/src/shaders/material-frag.glsl
  ${CMAKE_SOURCE_DIR}/src/shaders/matrix-transformed-vert.glsl
)
ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" VRShell_SOURCES)

set(VRShell_EXEFLAGS "")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
  set(VRShell_EXEFLAGS "MACOSX_BUNDLE")
elseif(WIN32)
  set(VRShell_EXEFLAGS WIN32)
endif()

add_executable(VRShell ${VRShell_EXEFLAGS} ${VRShell_SOURCES} ${VRShell_RESOURCES})
set_property(TARGET VRShell PROPERTY FOLDER "VRShell")
target_imported_libraries(VRShell PUBLIC Leap::Leap)

target_link_libraries(VRShell PUBLIC
  Autowiring 
  GLShaderLoader 
  OculusVR 
  graphics
  osinterface
  Primitives 
  SFMLController
  utility
)

if(WIN32)
  set_property(TARGET VRShell APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:libcmt.lib")
  set_property(TARGET VRShell PROPERTY VS_GLOBAL_ROOTNAMESPACE VRShell)
elseif(APPLE)

endif()

foreach(_resource ${VRShell_RESOURCES})
  if(WIN32)
    add_custom_command(TARGET VRShell POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different \"${_resource}\" \"$<TARGET_FILE_DIR:VRShell>\"
    )
  elseif(APPLE)
    get_filename_component(_resource ${_resource} ABSOLUTE)
    get_target_property(_is_bundle VRShell MACOSX_BUNDLE)
    if(_is_bundle)
      add_custom_command(TARGET VRShell POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:VRShell>/../Resources"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_resource}" "$<TARGET_FILE_DIR:VRShell>/../Resources"
      )
    endif()
  endif()
endforeach()
