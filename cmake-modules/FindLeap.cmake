#.rst
# FindLeap
# ------------
#
# Created by Walter Gray.
# Locate and configure Leap
#
# Interface Targets
# ^^^^^^^^^^^^^^^^^
#   Leap::Leap
#
# Variables
# ^^^^^^^^^
#   Leap_ROOT_DIR
#   Leap_FOUND
#   Leap_INCLUDE_DIR
#   Leap_LIBRARY
#   Leap_IMPORT_LIB
#   Leap_64_BIT - Set this to ON if you want to link with the 64 bit binaries

find_path(Leap_ROOT_DIR
          NAMES include/Leap.h
          HINTS ${EXTERNAL_LIBRARY_DIR}
          PATH_SUFFIXES LeapSDK-${Leap_FIND_VERSION}
                        LeapSDK)
#we should check the version.txt file here...

set(Leap_INCLUDE_DIR "${Leap_ROOT_DIR}/include")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(_bit_suffix x64)
else()
  set(_bit_suffix x86)
endif()

if(MSVC)
  find_library(Leap_IMPORT_LIB_RELEASE "Leap.lib" HINTS "${Leap_ROOT_DIR}/lib/${_bit_suffix}")
  find_library(Leap_IMPORT_LIB_DEBUG "Leap.lib" HINTS "${Leap_ROOT_DIR}/lib/${_bit_suffix}")

  find_file(Leap_LIBRARY_RELEASE
            NAMES Leap.dll
            HINTS "${Leap_ROOT_DIR}/lib/x86")
  find_file(Leap_LIBRARY_DEBUG
            NAMES Leapd.dll
                  Leap.dll #fallback on the release library if we must
            HINTS "${Leap_ROOT_DIR}/lib/x86")
  mark_as_advanced(Leap_IMPORT_LIB_RELEASE Leap_IMPORT_LIB_DEBUG)
elseif(APPLE)
  string(FIND "${CMAKE_CXX_FLAGS}" "-stdlib=libc++" found_lib)

  if(${found_lib} GREATER -1)
    set(_libdir ${Leap_ROOT_DIR}/lib)
  else()
    message(WARNING "Could not locate the library directory")
  endif()

  find_library(Leap_LIBRARY_RELEASE
            NAMES libLeap.dylib
            HINTS "${_libdir}")
  find_library(Leap_LIBRARY_DEBUG
            NAMES libLeapd.dylib
                  libLeap.dylib #fallback on the release library
            HINTS "${_libdir}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  find_library(Leap_LIBRARY_RELEASE
            NAMES libLeap.so
            HINTS "${Leap_ROOT_DIR}/lib/${_bit_suffix}")
  find_library(Leap_LIBRARY_DEBUG
            NAMES libLeapd.so
                  libLeap.so #fallback on the release library
            HINTS "${Leap_ROOT_DIR}/lib/${_bit_suffix}")
else()
  message(WARNING "FindLeap.cmake has not been implemented for this platform.")
endif()

# set(Leap_LIBRARY ${Leap_LIBRARY_RELEASE})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Leap DEFAULT_MSG Leap_ROOT_DIR Leap_INCLUDE_DIR Leap_LIBRARY_RELEASE Leap_LIBRARY_DEBUG)

include(CreateImportTargetHelpers)
generate_import_target(Leap SHARED)
