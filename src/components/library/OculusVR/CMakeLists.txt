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
    BRIEF_DOC_STRING
        "Provides Oculus VR tracking and distortion functionality."
)