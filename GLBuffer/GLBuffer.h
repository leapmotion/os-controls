#pragma once

#include "gl_glext_glu.h"
#include <string>

class GLBuffer {

public:

  GLBuffer();
  void create(GLenum type);
  void bind();
  void allocate(const void* data, int count, GLenum pattern);
  void release();
  int size() const;
  void* map(GLuint access);
  bool unmap();
  bool isCreated() const;
  void destroy();
  static void checkError(const std::string& loc = "");
  static void checkFrameBufferStatus(const std::string& loc = "");

private:

  GLuint buffer_;
  GLenum type_;

};
