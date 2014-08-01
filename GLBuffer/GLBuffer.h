#pragma once

#include "gl_glext_glu.h"
#include <string>

class GLBuffer {

public:

  GLBuffer();
  void Create(GLenum type);
  void Bind();
  void Allocate(const void* data, int count, GLenum pattern);
  void Release();
  int Size() const;
  void* Map(GLuint access);
  bool Unmap();
  bool IsCreated() const;
  void Destroy();

  static void CheckError(const std::string& loc = "");
  static void CheckFrameBufferStatus(const std::string& loc = "");

private:

  GLuint m_BufferAddress;
  GLenum m_BufferType;

};
