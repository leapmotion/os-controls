setlocal EnableExtensions EnableDelayedExpansion
set i=0;
for /F %%a in (%1) do set /A i+=1 & set files[!i!]=%%a
set i=0;
for /F %%b in (%2) do set /A i+=1 & set dirs[!i!]=%%b
set n=%i%;

cd %3

for /L %%i in (1,1,%n%) do (
  cmake -E make_directory !dirs[%%i]!
  cmake -E copy_if_different !files[%%i]! !dirs[%%i]!
)

::${CMAKE_COMMAND} -E copy_if_different \"%%i\" \"$<TARGET_FILE_DIR:${target}>/.\")