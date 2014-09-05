#include "ResourceManagerInitializer.h"

#include "Singleton.h"
#include "GLShaderLoader.h"
#include "TextFileLoader.h"

ResourceManagerInitializer::ResourceManagerInitializer(const char* basePath)
{
  Singleton<ResourceManager<GLShader>>::CreateInstance(basePath);
  Singleton<ResourceManager<GLShaderLoadParams>>::CreateInstance(basePath);
  Singleton<ResourceManager<TextFile>>::CreateInstance(basePath);
}


ResourceManagerInitializer::~ResourceManagerInitializer()
{
}
