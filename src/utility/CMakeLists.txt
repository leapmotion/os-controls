set(utility_SOURCES
  utility.h
  utility.cpp
  ComInitializer.h
  ComInitializer.cpp
  Config.h
  Config.cpp
  NativeWindow.h
)

add_windows_sources(utility_SOURCES 
  NativeWindowWin.cpp
)

add_mac_sources(utility_SOURCES
  NativeWindowMac.cpp
)
if(APPLE)
  set_property(SOURCE NativeWindowMac.cpp PROPERTY COMPILE_FLAGS "-x objective-c++")
endif()

add_library(utility ${utility_SOURCES})
target_link_libraries(utility Autowiring)
target_include_directories(utility PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)
