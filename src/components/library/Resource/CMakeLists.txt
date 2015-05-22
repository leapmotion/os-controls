add_sublibrary(
    Resource
    HEADERS
        Resource.h
    INTERNAL_DEPENDENCIES
        ResourceManager
        Singleton
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "Convenient typed, ResourceManager-loaded resources via singleton."
)