set(oscontrols_SOURCES
  oscontrols.h
  oscontrols.cpp
)

if(WIN32)
  set(oscontrols_EXEFLAGS WIN32)
elseif(APPLE)
  set_property(SOURCE oscontrols.cpp PROPERTY COMPILE_FLAGS "-x objective-c++")
  set(oscontrols_EXEFLAGS MACOSX_BUNDLE)
endif()

add_executable(oscontrols ${oscontrols_EXEFLAGS} ${oscontrols_SOURCES})
target_link_libraries(oscontrols Autowiring utility interaction graphics osinterface)
target_include_directories(oscontrols PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

target_package(oscontrols Leap REQUIRED)
target_package(oscontrols SFML 2.1 REQUIRED)

if(APPLE)
  set_target_properties(oscontrols PROPERTIES
                        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/src/oscontrols/oscontrols.plist.in)
endif()
