#pragma once

#include "gl_glext_glu.h"
#include <string>

// A C++ wrapper class for OpenGL buffer objects, of types e.g. GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER.
class GLBuffer {

public:

  GLBuffer();
  void Create(GLenum type);
  void Bind() const;
  void Unbind () const;
  void Allocate(const void* data, int count, GLenum pattern);
  void Write(const void* data, int count);
  int Size() const;
  void* Map(GLuint access);
  bool Unmap();
  bool IsCreated() const;
  void Destroy();

private:

  GLuint m_BufferAddress;
  GLenum m_BufferType;
};
