#pragma once

#include "ResourceManager.h"

namespace Leap {
namespace GL {

class GLTexture2;

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.

// TODO: maybe make a loader for GLTexture2Params, which would define "profiles" for
// different classes of textures (e.g. non-mipmapped RGB textures, 16-bit cubemaps, etc),
// so that loading a GLTexture2 would require a profile name and a file name.

// Template specialization of ResourceLoader<GLTexture2> which defines how to load such a resource.
template <>
struct ResourceLoader<GLTexture2> {
  static const bool exists = true;
  static std::shared_ptr<GLTexture2> LoadResource (const std::string &name, ResourceManager<GLTexture2> &calling_manager);
};
