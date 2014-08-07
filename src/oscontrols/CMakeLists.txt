set(oscontrols_SOURCES
  oscontrols.h
  oscontrols.cpp
)

if(WIN32)
  set(oscontrols_EXEFLAGS WIN32)
endif()

add_executable(oscontrols ${oscontrols_EXEFLAGS} ${oscontrols_SOURCES})
target_link_libraries(oscontrols Autowiring utility interaction graphics osinterface)
target_include_directories(oscontrols PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

target_package(oscontrols Leap REQUIRED )
target_package(oscontrols SFML 2.1 REQUIRED )

