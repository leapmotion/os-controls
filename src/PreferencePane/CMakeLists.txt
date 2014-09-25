if(NOT APPLE)
  return()
endif()

set(PreferencePane_SOURCES
  PreferencePane.h
  PreferencePane.mm
)

set(PreferencePane_RESOURCES
  ../Shortcuts/icons/ShortcutsPreferences.tiff
)

add_conditional_sources(PreferencePane_SOURCES "" GROUP_NAME "Resource Files" "FILES" ${PreferencePane_RESOURCES})

file(GLOB PreferencePane_XIBS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} Base.lproj/*.xib)
file(GLOB PreferencePane_STRINGS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.lproj/*.strings)
add_conditional_sources(PreferencePane_SOURCES "" GROUP_NAME "XIBs" "FILES" ${PreferencePane_XIBS} ${PreferencePane_STRINGS})

add_library(ShortcutsPreferences SHARED ${PreferencePane_SOURCES})
find_library(PREFERENCEPANES_FRAMEWORK PreferencePanes)
find_library(APPKIT_FRAMEWORK AppKit)
mark_as_advanced(PREFERENCEPANES_FRAMEWORK)
mark_as_advanced(APPKIT_FRAMEWORK)
target_link_libraries(ShortcutsPreferences utility ${PREFERENCEPANES_FRAMEWORK} ${APPKIT_FRAMEWORK})

#For version.h
target_include_directories(ShortcutsPreferences PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

configure_file(ShortcutsPreferences.plist.in Info.plist @ONLY)

set(SHORTCUTS_PREFERENCES_CONTENTS "Shortcuts.prefPane/Contents")

set_target_properties(ShortcutsPreferences PROPERTIES PREFIX "" SUFFIX ""
                      COMPILE_FLAGS "-fobjc-arc ${SHARED_FLAGS}"
                      LINK_FLAGS "-fobjc-arc -fobjc-link-runtime ${LINK_FLAGSr}"
                      LIBRARY_OUTPUT_DIRECTORY ${SHORTCUTS_PREFERENCES_CONTENTS}/MacOS/
                      LIBRARY_OUTPUT_DIRECTORY_DEBUG ${SHORTCUTS_PREFERENCES_CONTENTS}/MacOS/
                      LIBRARY_OUTPUT_DIRECTORY_RELEASE ${SHORTCUTS_PREFERENCES_CONTENTS}/MacOS/)

add_custom_command(TARGET ShortcutsPreferences PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${SHORTCUTS_PREFERENCES_CONTENTS}/Resources/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "Info.plist" ${SHORTCUTS_PREFERENCES_CONTENTS}/
)

foreach(_resource ${PreferencePane_RESOURCES})
  get_filename_component(_resource ${_resource} ABSOLUTE)
  add_custom_command(TARGET ShortcutsPreferences POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_resource} ${SHORTCUTS_PREFERENCES_CONTENTS}/Resources/
  )
endforeach()

foreach(_string ${PreferencePane_STRINGS})
  get_filename_component(_string_locale ${_string} PATH)
  get_filename_component(_string_name ${_string} NAME)
  get_filename_component(_string ${_string} ABSOLUTE)
  set(_resources_locale ${SHORTCUTS_PREFERENCES_CONTENTS}/Resources/${_string_locale}/)
  add_custom_command(TARGET ShortcutsPreferences POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${_resources_locale}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_string} ${_resources_locale}/${_string_name}
  )
endforeach()

find_program(IBTOOL ibtool HINTS "/usr/bin" "${OSX_DEVELOPER_ROOT}/usr/bin")
foreach(_xib ${PreferencePane_XIBS})
  get_filename_component(_nib_path ${_xib} PATH)
  get_filename_component(_nib_name ${_xib} NAME_WE)
  set(_resources_nib ${SHORTCUTS_PREFERENCES_CONTENTS}/Resources/${_nib_path}/)
  add_custom_command(TARGET ShortcutsPreferences POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${_resources_nib}
    COMMAND ${IBTOOL} --errors --warnings --notices --output-format human-readable-text --compile
    ${_resources_nib}${_nib_name}.nib
    ${CMAKE_CURRENT_SOURCE_DIR}/${_xib}
    COMMENT "Compiling src/PreferencePane/${_xib}"
  )
endforeach()
