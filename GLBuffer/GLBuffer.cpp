#include "GLBuffer.h"
#include <iostream>

GLBuffer::GLBuffer() : m_BufferAddress(0), m_BufferType(0) { }

void GLBuffer::Create(GLenum type) {
  m_BufferType = type;
  glGenBuffers(1, &m_BufferAddress);
  CheckError("Create");
}

void GLBuffer::Bind() {
  glBindBuffer(m_BufferType, m_BufferAddress);
  CheckError("Bind");
}

void GLBuffer::Allocate(const void* data, int count, GLenum pattern) {
  glBufferData(m_BufferType, count, data, pattern);
  CheckError("Allocate");
}

void GLBuffer::Release() {
  glBindBuffer(m_BufferType, 0);
  CheckError("Release");
}

int GLBuffer::Size() const {
  GLint value = -1;
  glGetBufferParameteriv(m_BufferType, GL_BUFFER_SIZE, &value);
  CheckError("Size");
  return value;
}

void* GLBuffer::Map(GLuint access) {
  void* ptr = glMapBufferARB(m_BufferType, access);
  CheckError("Map");
  return ptr;
}

bool GLBuffer::Unmap() {
  bool result = glUnmapBufferARB(m_BufferType) == GL_TRUE;
  CheckError("Unmap");
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
    std::cout << "GL error ";
    if (!loc.empty()) {
      std::cout << "at " << loc << ": ";
    }
    std::cout << "code: " << std::hex << err << std::endl;
    throw std::runtime_error("CheckError failed");
  }
}

void GLBuffer::CheckFrameBufferStatus(const std::string& loc) {
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer error ";
    if (!loc.empty()) {
      std::cout << "at " << loc << ": ";
    }
    std::cout << "code: " << std::hex << status << std::endl;
    throw std::runtime_error("CheckFrameBufferStatus failed");
  }
}
