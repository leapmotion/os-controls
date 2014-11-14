add_sublibrary(
    OculusVR
    SOURCE_PATH
        .
    HEADERS
        OculusVR.h
    SOURCES
        OculusVR.cpp
    INTERNAL_DEPENDENCIES
        EigenTypes
        GLCompatibility
    EXTERNAL_DEPENDENCIES
        "OculusSDK"
    BRIEF_DOC_STRING
        "Provides Oculus VR tracking and distortion functionality."
)