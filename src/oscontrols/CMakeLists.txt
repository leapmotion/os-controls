set(oscontrols_SOURCES
  NativeUI.h
  NativeUI.cpp
  oscontrols.h
  oscontrols.cpp
)

set(oscontrols_RESOURCES
  ${CMAKE_SOURCE_DIR}/src/shaders/material-frag.glsl
  ${CMAKE_SOURCE_DIR}/src/shaders/matrix-transformed-vert.glsl
  ${CMAKE_SOURCE_DIR}/src/svgs/next-track-icon-extended-01.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/play_pause-icon-extended-01.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/prev-track-icon-extended-01.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/volume-icon-01.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/scroll-cursor-body.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/scroll-cursor-finger_right.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/scroll-cursor-finger_left.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/scroll-cursor-line.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/scroll-cursor-ghost.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/volume-notch-active.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/volume-notch-inactive.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/expose-icon-01.svg
)

set(oscontrols_CLR_SOURCES
  NativeUIWin.resx
  NativeUIWin.h
  NativeUIWin.cpp
  AssemblyInfo.cpp
  Resource.resx
)

add_windows_sources(
  oscontrols_SOURCES
  ${oscontrols_CLR_SOURCES}
)

add_mac_sources(
  oscontrols_SOURCES
  ApplicationDelegate.h
  ApplicationDelegate.mm
  MenubarController.h
  MenubarController.mm
  NativeUIMac.mm
)

if(APPLE)
  set(oscontrols_RESOURCES
    ${oscontrols_RESOURCES}
    icons/AppIcon.icns
    icons/TrayIcon.tiff
    icons/TrayIconInverted.tiff
  )
endif()

add_conditional_sources(oscontrols_SOURCES "" GROUP_NAME "Resource Files" "FILES" ${oscontrols_RESOURCES})

if(WIN32)
  set(oscontrols_EXEFLAGS WIN32)
  #adding CLR support as described in:
  #http://www.cmake.org/pipermail/cmake/2010-June/037346.html
  set(old_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  set(old_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

  STRING(REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  STRING(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

  foreach(oscontrols_CLR_SOURCE ${oscontrols_CLR_SOURCES})
    get_filename_component(srcext ${oscontrols_CLR_SOURCE} EXT)
    if(${srcext} STREQUAL ".cpp")
      set_source_files_properties(${oscontrols_CLR_SOURCE} PROPERTIES COMPILE_FLAGS "/clr")
    endif()
  endforeach()

elseif(APPLE)
  set(oscontrols_EXEFLAGS MACOSX_BUNDLE)
  set(oscontrols_XIB_NAMES MainMenu)
  set(oscontrols_XIBS "")
  file(GLOB locales RELATIVE ${CMAKE_SOURCE_DIR}/src/oscontrols *.lproj)
  foreach(locale ${locales})
    foreach(xib ${oscontrols_XIB_NAMES})
      list(APPEND oscontrols_XIBS ${locale}/${xib}.xib)
    endforeach()
  endforeach()
  add_conditional_sources(oscontrols_SOURCES "" GROUP_NAME "XIBs" "FILES" ${CMAKE_SOURCE_DIR}/src/oscontrols/${oscontrols_XIBS})
endif()

add_executable(oscontrols ${oscontrols_EXEFLAGS} ${oscontrols_SOURCES})
target_link_libraries(oscontrols Autowiring utility interaction graphics osinterface expose)
target_include_directories(oscontrols PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

target_package(oscontrols Leap REQUIRED)
target_package(oscontrols SFML 2.1 REQUIRED)

if(WIN32)
  set_property(TARGET oscontrols APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:libcmt.lib")
  set_property(TARGET oscontrols APPEND PROPERTY VS_DOTNET_REFERENCES "System;System.Data;System.Drawing;System.Windows.Forms;System.Xml")
  set_target_properties(oscontrols PROPERTIES VS_GLOBAL_ROOTNAMESPACE oscontrols)
elseif(APPLE)
  set_target_properties(oscontrols PROPERTIES
                        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/src/oscontrols/oscontrols.plist.in)
endif()

foreach(_resource ${oscontrols_RESOURCES})
  if(WIN32)
    add_custom_command(TARGET oscontrols POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different \"${_resource}\" \"$<TARGET_FILE_DIR:oscontrols>\"
    )
    add_custom_command(TARGET oscontrols POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different \"${_resource}\" "${CMAKE_CURRENT_BINARY_DIR}"
    )
  elseif(APPLE)
    get_filename_component(_resource ${_resource} ABSOLUTE)
    get_target_property(_is_bundle oscontrols MACOSX_BUNDLE)
    if(_is_bundle)
      add_custom_command(TARGET oscontrols POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory \"$<TARGET_FILE_DIR:oscontrols>/../Resources/\"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different \"${_resource}\" \"$<TARGET_FILE_DIR:oscontrols>/../Resources/\"
      )
    endif()
  endif()
endforeach()

if(APPLE)
  find_program(IBTOOL ibtool HINTS "/usr/bin" "${OSX_DEVELOPER_ROOT}/usr/bin")
  foreach(xib ${oscontrols_XIBS})
    get_filename_component(nib_path ${xib} PATH)
    get_filename_component(nib_name ${xib} NAME_WE)
    add_custom_command(TARGET oscontrols POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory \"$<TARGET_FILE_DIR:oscontrols>/../Resources/${nib_path}\"
      COMMAND ${IBTOOL} --errors --warnings --notices --output-format human-readable-text --compile
      \"$<TARGET_FILE_DIR:oscontrols>/../Resources/${nib_path}/${nib_name}.nib\"
      ${CMAKE_SOURCE_DIR}/src/oscontrols/${xib}
      COMMENT "Compiling src/oscontrols/${xib}"
    )
  endforeach()
endif()
