add_sublibrary(
    EigenTypes # TODO: this should probably be renamed to just Eigen
    HEADERS
        EigenTypes.h
    INTERNAL_DEPENDENCIES
        C++11
    EXTERNAL_DEPENDENCIES
        "Eigen 3.2.1"
    BRIEF_DOC_STRING
        "Convenience typedefs for working with particular Eigen vector/matrix types."
)
add_subdirectory(Test)