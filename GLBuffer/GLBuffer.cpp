#include "GLBuffer.h"

#include "GLError.h"

GLBuffer::GLBuffer() : m_BufferAddress(0), m_BufferType(0) { }

void GLBuffer::Create(GLenum type) {
  m_BufferType = type;
  GL_THROW_UPON_ERROR(glGenBuffers(1, &m_BufferAddress));
}

void GLBuffer::Bind() const {
  GL_THROW_UPON_ERROR(glBindBuffer(m_BufferType, m_BufferAddress));
}

void GLBuffer::Unbind() const {
  GL_THROW_UPON_ERROR(glBindBuffer(m_BufferType, 0));
}

void GLBuffer::Write(const void* data, int count) {
  GL_THROW_UPON_ERROR(glBufferSubData(m_BufferType, 0, count, data));
}

void GLBuffer::Allocate(const void* data, int count, GLenum pattern) {
  GL_THROW_UPON_ERROR(glBufferData(m_BufferType, count, data, pattern));
}

int GLBuffer::Size() const {
  GLint value = -1;
  GL_THROW_UPON_ERROR(glGetBufferParameteriv(m_BufferType, GL_BUFFER_SIZE, &value));
  return value;
}

void* GLBuffer::Map(GLuint access) {
  Bind();
  GL_THROW_UPON_ERROR(void *ptr = glMapBufferARB(m_BufferType, access));
  Unbind();
  return ptr;
}

bool GLBuffer::Unmap() {
  Bind();
  GL_THROW_UPON_ERROR(bool result = glUnmapBufferARB(m_BufferType) == GL_TRUE);
  Unbind();
  return result;
}

bool GLBuffer::IsCreated() const {
  return m_BufferAddress != 0;
}

void GLBuffer::Destroy() {
  GL_THROW_UPON_ERROR(glDeleteBuffers(1, &m_BufferAddress));
  m_BufferAddress = 0;
}
