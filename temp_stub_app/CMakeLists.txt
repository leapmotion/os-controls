cmake_minimum_required(VERSION 2.8)
if(COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 NEW)
  if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
  endif()
endif()


add_executable(Main
Main.cpp
StubApplication.h
StubApplication.cpp
ExampleLayer.h
ExampleLayer.cpp)

include_directories(
..
../Application
../GLCompatibility
../GLController
../GLShader
../GLShaderLoader
../GLTexture2
../GLTexture2FreeImage
../GLTexture2Loader
../Resource
../ResourceManager
../SDLController
../TextAndBinaryFile
../TextAndBinaryFileLoader
)


# SDL2
find_path(SDL_DIR PATHS)
if(SDL_DIR STREQUAL "SDL_DIR-NOTFOUND")
  message(STATUS "SDL2 library not found, please specify its path (it should be called SDL2-2.0.x)")
  return()
endif()

include_directories(
  ${SDL_DIR}/include
)
add_library(sdl STATIC IMPORTED)
set_property(TARGET sdl PROPERTY IMPORTED_LOCATION ${SDL_DIR}/lib/SDL2.lib)

install(FILES ${SDL_DIR}/lib/SDL2.dll DESTINATION ${PROJECT_BINARY_DIR}/Debug)
install(FILES ${SDL_DIR}/lib/SDL2.dll DESTINATION ${PROJECT_BINARY_DIR}/Release)

target_link_libraries(Main sdl opengl32 glu32)