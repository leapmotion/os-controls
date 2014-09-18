set(Shortcuts_SOURCES
  NativeUI.h
  NativeUI.cpp
  Shortcuts.h
  Shortcuts.cpp
  Version.h
)

set(Shortcuts_RESOURCES
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
  ${CMAKE_SOURCE_DIR}/src/svgs/plus-icon.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/minus-icon.svg
  ${CMAKE_SOURCE_DIR}/src/svgs/disabled-cursor.svg
  ${CMAKE_SOURCE_DIR}/src/Shortcuts/config.json
)

set(Shortcuts_CLR_SOURCES
  NativeUIWin.resx
  NativeUIWin.h
  NativeUIWin.cpp
  AssemblyInfo.cpp
  Resource.resx
  Resource.rc
  resource.h
)

add_windows_sources(
  Shortcuts_SOURCES
  ${Shortcuts_CLR_SOURCES}
)

add_mac_sources(
  Shortcuts_SOURCES
  ApplicationDelegate.h
  ApplicationDelegate.mm
  MenubarController.h
  MenubarController.mm
  NativeUIMac.mm
)

if(APPLE)
  set(Shortcuts_RESOURCES
    ${Shortcuts_RESOURCES}
    icons/AppIcon.icns
    icons/TrayIcon.tiff
    icons/TrayIconInverted.tiff
  )
endif()

add_conditional_sources(Shortcuts_SOURCES "" GROUP_NAME "Resource Files" "FILES" ${Shortcuts_RESOURCES})

if(WIN32)
  set(Shortcuts_EXEFLAGS WIN32)
  #adding CLR support as described in:
  #http://www.cmake.org/pipermail/cmake/2010-June/037346.html
  set(old_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  set(old_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

  STRING(REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  STRING(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

  foreach(Shortcuts_CLR_SOURCE ${Shortcuts_CLR_SOURCES})
    get_filename_component(srcext ${Shortcuts_CLR_SOURCE} EXT)
    if(${srcext} STREQUAL ".cpp")
      set_source_files_properties(${Shortcuts_CLR_SOURCE} PROPERTIES COMPILE_FLAGS "/clr")
    endif()
  endforeach()

elseif(APPLE)
  set(Shortcuts_EXEFLAGS MACOSX_BUNDLE)
  set(Shortcuts_XIB_NAMES MainMenu)
  set(Shortcuts_XIBS "")
  file(GLOB locales RELATIVE ${CMAKE_SOURCE_DIR}/src/Shortcuts *.lproj)
  foreach(locale ${locales})
    foreach(xib ${Shortcuts_XIB_NAMES})
      list(APPEND Shortcuts_XIBS ${locale}/${xib}.xib)
    endforeach()
  endforeach()
  add_conditional_sources(Shortcuts_SOURCES "" GROUP_NAME "XIBs" "FILES" ${CMAKE_SOURCE_DIR}/src/Shortcuts/${Shortcuts_XIBS})
endif()

configure_file(Version.h.in Version.h)

add_executable(Shortcuts ${Shortcuts_EXEFLAGS} ${Shortcuts_SOURCES})
target_link_libraries(Shortcuts Autowiring utility interaction graphics osinterface expose)
target_include_directories(Shortcuts PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

#For version.h
target_include_directories(Shortcuts PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_package(Shortcuts Leap REQUIRED)
target_package(Shortcuts SFML 2.1 REQUIRED)

if(WIN32)
  set_property(TARGET Shortcuts APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:libcmt.lib")
  set_property(TARGET Shortcuts APPEND PROPERTY VS_DOTNET_REFERENCES "System;System.Data;System.Drawing;System.Windows.Forms;System.Xml")
  set_target_properties(Shortcuts PROPERTIES VS_GLOBAL_ROOTNAMESPACE Shortcuts)
  set_property(TARGET Shortcuts APPEND PROPERTY RESOURCE ${Shortcuts_RESOURCES})
elseif(APPLE)
  set_target_properties(Shortcuts PROPERTIES
                        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/src/Shortcuts/Shortcuts.plist.in)
endif()


foreach(_resource ${Shortcuts_RESOURCES})
  if(WIN32)
    add_custom_command(TARGET Shortcuts POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different \"${_resource}\" \"$<TARGET_FILE_DIR:Shortcuts>\"
    )
    add_custom_command(TARGET Shortcuts POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different \"${_resource}\" "${CMAKE_CURRENT_BINARY_DIR}"
    )
  elseif(APPLE)
    get_filename_component(_resource ${_resource} ABSOLUTE)
    get_target_property(_is_bundle Shortcuts MACOSX_BUNDLE)
    if(_is_bundle)
      add_custom_command(TARGET Shortcuts POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory \"$<TARGET_FILE_DIR:Shortcuts>/../Resources/\"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different \"${_resource}\" \"$<TARGET_FILE_DIR:Shortcuts>/../Resources/\"
      )
    endif()
  endif()
endforeach()

if(APPLE)
  find_program(IBTOOL ibtool HINTS "/usr/bin" "${OSX_DEVELOPER_ROOT}/usr/bin")
  foreach(xib ${Shortcuts_XIBS})
    get_filename_component(nib_path ${xib} PATH)
    get_filename_component(nib_name ${xib} NAME_WE)
    add_custom_command(TARGET Shortcuts POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory \"$<TARGET_FILE_DIR:Shortcuts>/../Resources/${nib_path}\"
      COMMAND ${IBTOOL} --errors --warnings --notices --output-format human-readable-text --compile
      \"$<TARGET_FILE_DIR:Shortcuts>/../Resources/${nib_path}/${nib_name}.nib\"
      ${CMAKE_SOURCE_DIR}/src/Shortcuts/${xib}
      COMMENT "Compiling src/Shortcuts/${xib}"
    )
  endforeach()
endif()

# Install commands for the Resources
install(TARGETS Shortcuts
  RUNTIME DESTINATION "."
  BUNDLE DESTINATION "."
  RESOURCE DESTINATION "."
)
