#pragma once

#include "ResourceManager.h"

namespace Leap {
namespace GL {

class Texture2;

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL;

// TODO: maybe make a loader for Texture2Params, which would define "profiles" for
// different classes of textures (e.g. non-mipmapped RGB textures, 16-bit cubemaps, etc),
// so that loading a Texture2 would require a profile name and a file name.

// Template specialization of ResourceLoader<Texture2> which defines how to load such a resource.
template <>
struct ResourceLoader<Texture2> {
  static const bool exists = true;
  static std::shared_ptr<Texture2> LoadResource (const std::string &name, ResourceManager<Texture2> &calling_manager);
};
