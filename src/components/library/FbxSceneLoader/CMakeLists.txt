add_sublibrary(
    FbxSceneLoader
    HEADERS
        FbxSceneLoader.h
    SOURCES
        FbxSceneLoader.cpp
    EXPLICIT_SUBLIBRARY_DEPENDENCIES
        C++11
        ResourceManager
    EXPLICIT_LIBRARY_DEPENDENCIES
        "FbxSdk" # TODO: specify version (IMPORTANT: inside the quotes, e.g. "FbxSdk 2015.1")
    BRIEF_DOC_STRING
        "ResourceManager-managed loading of FbxScene objects using the FBX SDK."
)