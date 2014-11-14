add_sublibrary(
    FbxSceneLoader
    SOURCE_PATH
        .
    HEADERS
        FbxSceneLoader.h
    SOURCES
        FbxSceneLoader.cpp
    INTERNAL_DEPENDENCIES
        C++11
        ResourceManager
    EXTERNAL_DEPENDENCIES
        "FbxSdk" # TODO: specify version (IMPORTANT: inside the quotes, e.g. "FbxSdk 2015.1")
    BRIEF_DOC_STRING
        "ResourceManager-managed loading of FbxScene objects using the FBX SDK."
)