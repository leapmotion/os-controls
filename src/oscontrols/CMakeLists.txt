set(oscontrols_SOURCES
  oscontrols.h
  oscontrols.cpp
)

set(oscontrols_RESOURCES
  ../shaders/default-frag.glsl
  ../shaders/default-vert.glsl
)


add_conditional_sources(oscontrols_SOURCES "" GROUP_NAME "Resource Files" "FILES" ${oscontrols_RESOURCES})

if(WIN32)
  set(oscontrols_EXEFLAGS WIN32)
elseif(APPLE)
  set_property(SOURCE oscontrols.cpp PROPERTY COMPILE_FLAGS "-x objective-c++")
  set(oscontrols_EXEFLAGS MACOSX_BUNDLE)
  set(MACOSX_BUNDLE_BUNDLE_NAME "oscontrols")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "1")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "0.0.1")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "0.0.1")
  set(MACOSX_BUNDLE_COPYRIGHT "Copyright © 2014 Leap Motion, Inc. All rights reserved.")
endif()

add_executable(oscontrols ${oscontrols_EXEFLAGS} ${oscontrols_SOURCES})
set_property(TARGET oscontrols APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:libcmt.lib")
target_link_libraries(oscontrols Autowiring utility interaction graphics osinterface)
target_include_directories(oscontrols PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

target_package(oscontrols Leap REQUIRED )
target_package(oscontrols SFML 2.1 REQUIRED )

foreach(_resource ${oscontrols_RESOURCES}) 
  if(WIN32)
    add_custom_command(TARGET oscontrols POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different \"${_resource}\" \"$<TARGET_FILE_DIR:oscontrols>\"
    )
  elseif()
    get_target_property(_is_bundle ${target} MACOSX_BUNDLE)
    if(_is_bundle)
      add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory \"$<TARGET_FILE_DIR:oscontrols>/../Resources/\"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different \"${_resource}\" \"$<TARGET_FILE_DIR:oscontrols>/../Resources/\"
      )
    endif()
  endif()

endforeach()
