add_sublibrary(
    OculusVR
    SOURCE_PATH
        .
    HEADERS
        OculusVR.h
    SOURCES
        OculusVR.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        EigenTypes
        GLCompatibility
    EXPLICIT_LIBRARY_DEPENDENCIES
        "OculusSDK"
    BRIEF_DOC_STRING
        "Provides Oculus VR tracking and distortion functionality."
)