add_sublibrary(
    OculusVR
    HEADERS
        OculusVR.h
    SOURCES
        OculusVR.cpp
    INTERNAL_DEPENDENCIES
        EigenTypes
        LeapGLCore
    EXTERNAL_DEPENDENCIES
        "OculusSDK"
    DEBUG_EXPORT_NAME
        ComponentsDebug
    RELEASE_EXPORT_NAME
        ComponentsRelease
    BRIEF_DOC_STRING
        "Provides Oculus VR tracking and distortion functionality."
)