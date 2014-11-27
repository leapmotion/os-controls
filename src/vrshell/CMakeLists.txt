set(VRShell_VERSION_MAJOR "0")
set(VRShell_VERSION_MINOR "1")
set(VRShell_VERSION_PATCH "0")
set(VRShell_VERSION "${VRShell_VERSION_MAJOR}.${VRShell_VERSION_MINOR}.${VRShell_VERSION_PATCH}")
set(VRShell_APPNAME "Quick Switch")
set(VRShell_APPDESC "Leap Motion Quick Switch")
set(LEAP_COMPANY_NAME "Leap Motion")

set(VRShell_SOURCES
  AROverlay.h
  AROverlay.cpp
  LeapImagePassthrough.h
  LeapImagePassthrough.cpp
  NativeUI.h
  NativeUI.cpp
  Version.h.in
  Version.h
  VRShell.h
  VRShell.cpp
)

set(VRShell_RESOURCES
  ${CMAKE_SOURCE_DIR}/src/shaders/material-frag.glsl
  ${CMAKE_SOURCE_DIR}/src/shaders/matrix-transformed-vert.glsl
  ${CMAKE_SOURCE_DIR}/src/shaders/passthrough-frag.glsl
  ${CMAKE_SOURCE_DIR}/src/shaders/passthrough-vert.glsl
)
add_pch(VRShell_SOURCES "stdafx.h" "stdafx.cpp")

set(VRShell_CLR_SOURCES
  AssemblyInfo.cpp
  NativeUIWin.resx #Required by CMake for the .h to be editable in the designer.
  NativeUIWin.h
  NativeUIWin.cpp
  Resources.resx
  Resources.rc
  Resources.h
)

add_windows_sources(VRShell_SOURCES ${VRShell_CLR_SOURCES})
add_mac_sources(VRShell_SOURCES NativeUIMac.mm)

#adding CLR support as described in:
#http://www.cmake.org/pipermail/cmake/2010-June/037346.html

if(WIN32)
  set(old_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  set(old_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

  STRING(REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  STRING(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

  foreach(VRShell_CLR_SOURCE ${VRShell_CLR_SOURCES})
    get_filename_component(srcext ${VRShell_CLR_SOURCE} EXT)
    if(${srcext} STREQUAL ".cpp")
      set_source_files_properties(${VRShell_CLR_SOURCE} PROPERTIES COMPILE_FLAGS "/clr")
    endif()
  endforeach()
endif()

add_resource_files(VRShell_SOURCES
  ${VRShell_RESOURCES}
  icons/icon.ico
  icons/icon_512x512.png
)

configure_file(Version.h.in Version.h)

set(VRShell_EXEFLAGS "")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
  set(VRShell_EXEFLAGS "MACOSX_BUNDLE")
elseif(WIN32)
  set(VRShell_EXEFLAGS WIN32)
endif()

add_executable(VRShell ${VRShell_EXEFLAGS} ${VRShell_SOURCES} ${VRShell_RESOURCES})
set_property(TARGET VRShell PROPERTY FOLDER "VRShell")

#For version.h
target_include_directories(VRShell PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_imported_libraries(VRShell PUBLIC Leap::Leap)

target_link_libraries(VRShell PUBLIC
  Autowiring
  GLShaderLoader
  graphics
  hmdinterface
  interaction
  osinterface
  Primitives
  utility
)

if(WIN32)
  set_property(TARGET VRShell APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:libcmt.lib")
  set_property(TARGET VRShell APPEND PROPERTY VS_DOTNET_REFERENCES "System;System.Data;System.Drawing;System.Windows.Forms;System.Xml")
  set_property(TARGET VRShell PROPERTY VS_GLOBAL_ROOTNAMESPACE VRShell)
elseif(APPLE)
  find_library(IOKIT_FRAMEWORK IOKit)
  mark_as_advanced(IOKIT_FRAMEWORK)
  target_link_libraries(VRShell PUBLIC ${IOKIT_FRAMEWORK})
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
