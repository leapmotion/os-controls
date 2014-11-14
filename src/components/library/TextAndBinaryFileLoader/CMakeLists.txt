add_sublibrary(
    TextAndBinaryFileLoader
    SOURCE_PATH
        .
    HEADERS
        TextFileLoader.h
    INTERNAL_DEPENDENCIES
        ResourceManager
        TextAndBinaryFile
    BRIEF_DOC_STRING
        "ResourceManager-managed loading of text and binary files."
)