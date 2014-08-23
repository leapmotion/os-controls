#include "GLBuffer.h"

#include <stdexcept>
#include <sstream>

GLBuffer::GLBuffer() : m_BufferAddress(0), m_BufferType(0) { }

void GLBuffer::Create(GLenum type) {
  m_BufferType = type;
  glGenBuffers(1, &m_BufferAddress);
  CheckError("during GLBuffer::Create");
}

void GLBuffer::Bind() const {
  glBindBuffer(m_BufferType, m_BufferAddress);
  CheckError("during GLBuffer::Bind");
}

void GLBuffer::Unbind() const {
  glBindBuffer(m_BufferType, 0);
  CheckError("during GLBuffer::Release");
}

void GLBuffer::Allocate(const void* data, int count, GLenum pattern) {
  glBufferData(m_BufferType, count, data, pattern);
  CheckError("during GLBuffer::Allocate");
}

int GLBuffer::Size() const {
  GLint value = -1;
  glGetBufferParameteriv(m_BufferType, GL_BUFFER_SIZE, &value);
  CheckError("during GLBuffer::Size");
  return value;
}

void* GLBuffer::Map(GLuint access) {
  Bind();
  void* ptr = glMapBufferARB(m_BufferType, access);
  CheckError("during GLBuffer::Map");
  Unbind();
  return ptr;
}

bool GLBuffer::Unmap() {
  Bind();
  bool result = glUnmapBufferARB(m_BufferType) == GL_TRUE;
  CheckError("during GLBuffer::Unmap");
  Unbind();
  return result;
}

bool GLBuffer::IsCreated() const {
  return m_BufferAddress != 0;
}

void GLBuffer::Destroy() {
  glDeleteBuffers(1, &m_BufferAddress);
  m_BufferAddress = 0;
}

void GLBuffer::CheckError(const std::string& loc) {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    std::stringstream ss;
//     ss << "GL error \"" << gluErrorString(err) << '\"';
    ss << "GL error";
    if (!loc.empty()) {
      ss << " at " << loc << ":";
    }
    ss << " code: 0x" << std::hex << err;
    throw std::runtime_error(ss.str());
  }
}
