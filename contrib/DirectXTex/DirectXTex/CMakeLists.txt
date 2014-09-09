if(NOT WIN32)
  return()
endif()

set(directxtex_SOURCES
  BC.cpp
  BC4BC5.cpp
  BC6HBC7.cpp
  BCDirectCompute.cpp
  DirectXTexCompress.cpp
  DirectXTexCompressGPU.cpp
  DirectXTexConvert.cpp
  DirectXTexD3D11.cpp
  DirectXTexDDS.cpp
  DirectXTexFlipRotate.cpp
  DirectXTexImage.cpp
  DirectXTexMipmaps.cpp
  DirectXTexMisc.cpp
  DirectXTexNormalMaps.cpp
  DirectXTexPMAlpha.cpp
  DirectXTexResize.cpp
  DirectXTexTGA.cpp
  DirectXTexUtil.cpp
  DirectXTexWIC.cpp
  BC.h
  BCDirectCompute.h
  DDS.h
  DirectXTex.h
  DirectXTexP.h
  Filters.h
  scoped.h
)

add_library(directxtex ${directxtex_SOURCES})
target_include_directories(directxtex PUBLIC ..)