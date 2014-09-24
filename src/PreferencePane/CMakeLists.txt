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

file(GLOB _locales RELATIVE ${CMAKE_SOURCE_DIR}/src/PreferencePane *.lproj)
list(REMOVE_ITEM _locales "Base.lproj")
file(GLOB PreferencePane_XIBS RELATIVE ${CMAKE_SOURCE_DIR}/src/PreferencePane Base.lproj/*.xib)
add_conditional_sources(PreferencePane_SOURCES "" GROUP_NAME "XIBs" "FILES" ${PreferencePane_XIBS})

add_library(ShortcutsPreferences SHARED ${PreferencePane_SOURCES})
find_library(PREFERENCEPANES_FRAMEWORK PreferencePanes)
mark_as_advanced(PREFERENCEPANES_FRAMEWORK)
target_link_libraries(ShortcutsPreferences utility ${PREFERENCEPANES_FRAMEWORK})

#For version.h
target_include_directories(ShortcutsPreferences PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

set(SHORTCUTS_PREFERENCES_CONTENTS ${CMAKE_BINARY_DIR}/bin/Shortcuts.prefPane/Contents)

add_custom_command(TARGET ShortcutsPreferences PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${SHORTCUTS_PREFERENCES_CONTENTS}/Resources/
)

configure_file(${CMAKE_SOURCE_DIR}/src/PreferencePane/ShortcutsPreferences.plist.in
               ${SHORTCUTS_PREFERENCES_CONTENTS}/Info.plist @ONLY)

set_target_properties(ShortcutsPreferences PROPERTIES PREFIX "" SUFFIX ""
                      COMPILE_FLAGS "-fobjc-arc ${SHARED_FLAGS}"
                      LINK_FLAGS "-fobjc-arc -fobjc-link-runtime ${LINK_FLAGSr}"
                      LIBRARY_OUTPUT_DIRECTORY ${SHORTCUTS_PREFERENCES_CONTENTS}/MacOS/)

foreach(_resource ${PreferencePane_RESOURCES})
  get_filename_component(_resource ${_resource} ABSOLUTE)
  add_custom_command(TARGET ShortcutsPreferences POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different \"${_resource}\" ${SHORTCUTS_PREFERENCES_CONTENTS}/Resources/
  )
endforeach()

foreach(_locale ${_locales})
  set(_resources_locale ${SHORTCUTS_PREFERENCES_CONTENTS}/Resources/${_locale}/)
  get_filename_component(_locale ${_locale} ABSOLUTE)
  add_custom_command(TARGET ShortcutsPreferences POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory \"${_resources_locale}\"
    COMMAND ${CMAKE_COMMAND} -E copy_directory \"${_locale}/\" \"${_resources_locale}\"
  )
endforeach()

find_program(IBTOOL ibtool HINTS "/usr/bin" "${OSX_DEVELOPER_ROOT}/usr/bin")
foreach(_xib ${PreferencePane_XIBS})
  get_filename_component(_nib_path ${_xib} PATH)
  get_filename_component(_nib_name ${_xib} NAME_WE)
  set(_resources_nib ${SHORTCUTS_PREFERENCES_CONTENTS}/Resources/${_nib_path}/)
  add_custom_command(TARGET ShortcutsPreferences POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory \"${_resources_nib}\"
    COMMAND ${IBTOOL} --errors --warnings --notices --output-format human-readable-text --compile
    \"${_resources_nib}${_nib_name}.nib\"
    ${CMAKE_SOURCE_DIR}/src/PreferencePane/${_xib}
    COMMENT "Compiling src/PreferencePane/${_xib}"
  )
endforeach()
