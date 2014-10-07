#pragma once

#include "ResourceManager.h"
#include "fbxsdk.h"
#include <string>

class FbxSceneWrapper;

// Template specialization of ResourceLoader<FbxScene> which defines how to load such an FbxScene
template <>
struct ResourceLoader<FbxScene> {
  static const bool exists = true;
  static std::shared_ptr<FbxScene> LoadResource (const std::string &name, ResourceManager<FbxScene> &calling_manager);
};
