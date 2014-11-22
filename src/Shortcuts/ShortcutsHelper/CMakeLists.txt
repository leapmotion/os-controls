if(NOT APPLE)
  return()
endif()

set(ShortcutsHelper_SOURCES
  ShortcutsHelper.mm
)

add_executable(ShortcutsHelper ${ShortcutsHelper_SOURCES})
find_library(FOUNDATION_FRAMEWORK Foundation)
mark_as_advanced(FOUNDATION_FRAMEWORK)
target_link_libraries(ShortcutsHelper -lsqlite3 ${FOUNDATION_FRAMEWORK})
