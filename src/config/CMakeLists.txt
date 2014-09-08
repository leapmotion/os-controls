set(config_HEADERS configwindow.h)
set(config_SOURCES main.cpp configwindow.cpp)
set(config_UIFILES configwindow.ui)
#set(config_RESOURCES config.qrc)

if(WIN32)
  set(config_EXEFLAGS WIN32)
elseif(APPLE)
  set_property(SOURCE config.cpp PROPERTY COMPILE_FLAGS "-x objective-c++")
  set(config_EXEFLAGS MACOSX_BUNDLE)
endif()

qt5_wrap_ui(config_UI_SRCS ${config_UIFILES})
qt5_wrap_cpp(config_MOC_SOURCES ${config_HEADERS} ${config_SOURCES})

add_executable(config ${config_EXEFLAGS} 
  ${config_HEADERS}
  ${config_SOURCES}
  ${config_RESOURCES}
  ${config_UI_SRCS}
  ${config_MOC_SOURCES}
)

target_link_libraries(config Qt5::Widgets)
target_link_libraries(config Autowiring utility interaction graphics osinterface expose)
target_include_directories(config PUBLIC contrib/autowiring)

target_package(config Leap REQUIRED)

if(WIN32)
  set_property(TARGET config APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB:libcmt.lib")
elseif(APPLE)
  set_target_properties(config PROPERTIES
                        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/src/oscontrols/oscontrols.plist.in)
endif()

