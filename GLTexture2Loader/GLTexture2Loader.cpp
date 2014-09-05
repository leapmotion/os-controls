#include "GLTexture2Loader.h"

#include "GLTexture2.h"
#include "GLTexture2FreeImage.h"

#include <cassert>

std::shared_ptr<GLTexture2> ResourceLoader<GLTexture2>::LoadResource (const std::string &name, ResourceManager<GLTexture2> &calling_manager) {
  // TODO: once the GLTexture2Params loader has been made, these values would be loaded from it.
  // for now, use reasonable fixed values.
  GLTexture2Params params;
  params.SetTarget(GL_TEXTURE_2D);
  params.SetTexParameteri(GL_GENERATE_MIPMAP, GL_TRUE);
  params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  params.SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  params.SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  return std::shared_ptr<GLTexture2>(LoadGLTexture2UsingFreeImage(name, params));
}
