#include "Texture.h"

#define FREEIMAGE_LIB
#include "FreeImage.h"

#include <algorithm>

// Internal utility methods

/// Load an image given a filepath
FIBITMAP* load(const char * filepath)
{
  FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filepath, 0);

  if (format == FIF_UNKNOWN) {
    format = FreeImage_GetFIFFromFilename(filepath);
  }

  if (FreeImage_FIFSupportsReading(format)) {
    return FreeImage_Load(format, filepath);
  }

  // else
  return nullptr;
}

// static

Texture* Texture::Create(const char * path, const Format& format)
{
  // Load the image
  FIBITMAP* bitmap = load(path);
  if (!bitmap) {
    return nullptr;
  }
  
  unsigned height = FreeImage_GetHeight(bitmap);
  unsigned width = FreeImage_GetWidth(bitmap);
  const GLvoid * bits = FreeImage_GetBits(bitmap);

  // Create and return texture
  Texture* texture = new Texture(width, height, bits, format);

  FreeImage_Unload(bitmap);

  return texture;
}

Texture* Texture::Create(int width, int height, const Format& format)
{
  return new Texture(width, height, nullptr, format);
}

Texture* Texture::Create(unsigned char * data, int width, int height, int bytesPerLine, int bpp, const Format& format)
{
  FIBITMAP* bitmap = FreeImage_ConvertFromRawBits(data, width, height, bytesPerLine, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);
  
  const GLvoid * bits = FreeImage_GetBits(bitmap);

  // Create and return texture
  Texture* texture = new Texture(width, height, bits, format);

  FreeImage_Unload(bitmap);

  return texture;
}

void Texture::Initialize()
{
  FreeImage_Initialise();
}

// public

Texture::~Texture()
{
  glDeleteTextures(1, &m_TextureId);
}


void Texture::Bind() const
{
  glBindTexture(GL_TEXTURE_2D, m_TextureId);
}

GLuint Texture::Id() const
{
  return m_TextureId;
}


void Texture::Unbind() const
{
  glBindTexture(GL_TEXTURE_2D, 0);
}

// protected

Texture::Texture(int width, int height, const GLvoid* bits, const Format& format)
{
  // create GL texture
  glGenTextures(1, &m_TextureId);
  
  Bind();

  glTexImage2D(GL_TEXTURE_2D, format.mipmapLevel, format.internalFormat, width, height, 0, format.imageFormat, format.type, bits);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  Unbind();
}
