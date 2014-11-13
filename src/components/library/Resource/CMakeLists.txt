add_sublibrary(
    Resource
    SOURCE_PATH
        .
    HEADERS
        Resource.h
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        ResourceManager
        Singleton
    BRIEF_DOC_STRING
        "Convenient typed, ResourceManager-loaded resources via singleton."
)