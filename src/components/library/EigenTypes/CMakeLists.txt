add_sublibrary(
    EigenTypes # TODO: this should probably be renamed to just Eigen
    HEADERS
        EigenTypes.h
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
    EXPLICIT_LIBRARY_DEPENDENCIES
        "Eigen 3.2.1"
    BRIEF_DOC_STRING
        "Convenience typedefs for working with particular Eigen vector/matrix types."
)