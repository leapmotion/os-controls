set(cursor_SOURCES
  CursorView.h
  CursorView.cpp
)

add_pch(graphics_SOURCES "stdafx.h" "stdafx.cpp")

add_library(cursor ${cursor_EXEFLAGS} ${cursor_SOURCES})
set_property(TARGET cursor PROPERTY FOLDER "Shortcuts")
target_link_libraries(cursor Autowiring graphics)