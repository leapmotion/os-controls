if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(_cxx11_compiler_flags "-std=c++11")
endif()

add_sublibrary(
    C++11
    SOURCE_PATH
        "."
    COMPILE_OPTIONS
        ${_cxx11_compiler_flags}
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "A phony target which just provides compiler options for enabling C++11."
)

add_subdirectory(Test)
