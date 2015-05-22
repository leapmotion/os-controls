add_sublibrary(
    TextAndBinaryFileLoader
    HEADERS
        TextFileLoader.h
    INTERNAL_DEPENDENCIES
        ResourceManager
        TextAndBinaryFile
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "ResourceManager-managed loading of text and binary files."
)