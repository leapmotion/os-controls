set(cursor_SOURCES
  CursorView.h
  CursorView.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" graphics_SOURCES)

add_library(cursor ${cursor_EXEFLAGS} ${cursor_SOURCES})
set_property(TARGET cursor PROPERTY FOLDER "Shortcuts")
target_link_libraries(cursor Autowiring graphics)