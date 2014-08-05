set(oscontrols_SOURCES
  oscontrols.h
  oscontrols.cpp
)

if(WIN32)
  set(oscontrols_EXEFLAGS WIN32)
endif()

add_executable(oscontrols ${oscontrols_EXEFLAGS} ${oscontrols_SOURCES})
target_link_libraries(oscontrols Autowiring utility interaction engine osinterface)
target_include_directories(oscontrols PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

find_package(SFML 2.1 REQUIRED)
target_link_libraries(oscontrols LINK_PRIVATE ${SFML_LIBRARIES})
target_include_directories(oscontrols PRIVATE ${SFML_INCLUDE_DIR})


target_package(oscontrols SDL 2.0.1 REQUIRED)
target_package(oscontrols Leap REQUIRED)

