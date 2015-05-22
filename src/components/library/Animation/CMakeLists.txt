add_sublibrary(
    Animation
    SOURCE_PATH
        .
    HEADERS
        Animation.h
    INTERNAL_DEPENDENCIES
        EigenTypes
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "Basic templated animation and interpolation."
)
