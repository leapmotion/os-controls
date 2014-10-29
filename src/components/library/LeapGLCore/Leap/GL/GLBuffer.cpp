#include "Leap/GL/GLBuffer.h"

#include "Leap/GL/GLError.h"

namespace Leap {
namespace GL {

GLBuffer::GLBuffer() : m_BufferAddress(0), m_BufferType(0), m_SizeInBytes(0) { }

GLBuffer::~GLBuffer() {
  try {
    Destroy();
  } catch(...) {}
}

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

void GLBuffer::Allocate(const void* data, GLsizeiptr size_in_bytes, GLenum usage_pattern) {
  GL_THROW_UPON_ERROR(glBufferData(m_BufferType, size_in_bytes, data, usage_pattern));
  m_SizeInBytes = size_in_bytes;
}

void GLBuffer::Write(const void* data, int count) {
  GL_THROW_UPON_ERROR(glBufferSubData(m_BufferType, 0, count, data));
}

void* GLBuffer::Map(GLenum access) {
  Bind();
  GL_THROW_UPON_ERROR(void *ptr = glMapBuffer(m_BufferType, access));
  Unbind();
  return ptr;
}

bool GLBuffer::Unmap() {
  Bind();
  GL_THROW_UPON_ERROR(bool result = glUnmapBuffer(m_BufferType) == GL_TRUE);
  Unbind();
  return result;
}

bool GLBuffer::IsCreated() const {
  return m_BufferAddress != 0;
}

void GLBuffer::Destroy() {
  if (IsCreated()) {
    GL_THROW_UPON_ERROR(glDeleteBuffers(1, &m_BufferAddress));
    m_BufferAddress = 0;
    m_SizeInBytes = 0;
  }
}

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
