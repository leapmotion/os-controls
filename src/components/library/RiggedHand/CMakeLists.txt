add_sublibrary(
    RiggedHand
    HEADERS
        RiggedHand.h
    SOURCES
        RiggedHand.cpp
    INTERNAL_DEPENDENCIES
        Rigging
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "Realistic deformable hand."
)