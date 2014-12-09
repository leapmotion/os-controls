#include "Leap/GL/Buffer.h"

#include <cassert>
#include "Leap/GL/Error.h"

namespace Leap {
namespace GL {

Buffer::Buffer ()
  : m_BufferAddress(0)
  , m_BufferType(0)
  , m_SizeInBytes(0)
{ }

Buffer::Buffer (GLenum type)
  : m_BufferAddress(0)
  , m_BufferType(0)
  , m_SizeInBytes(0)
{
  Initialize(type);
}

Buffer::~Buffer () {
  Shutdown();
}

void Buffer::Initialize (GLenum type) {
  m_BufferType = type;
  THROW_UPON_GL_ERROR(glGenBuffers(1, &m_BufferAddress));
}

void Buffer::Shutdown () {
  if (IsInitialized()) {
    // THROW_UPON_GL_ERROR(glDeleteBuffers(1, &m_BufferAddress));
    glDeleteBuffers(1, &m_BufferAddress);
    m_BufferAddress = 0;
    m_SizeInBytes = 0;
    assert(!IsInitialized());
  }
}

void Buffer::Bind () const {
  THROW_UPON_GL_ERROR(glBindBuffer(m_BufferType, m_BufferAddress));
}

void Buffer::Unbind () const {
  THROW_UPON_GL_ERROR(glBindBuffer(m_BufferType, 0));
}

void Buffer::Allocate (const void* data, GLsizeiptr size_in_bytes, GLenum usage_pattern) {
  THROW_UPON_GL_ERROR(glBufferData(m_BufferType, size_in_bytes, data, usage_pattern));
  m_SizeInBytes = size_in_bytes;
}

void Buffer::Write (const void* data, int count) {
  THROW_UPON_GL_ERROR(glBufferSubData(m_BufferType, 0, count, data));
}

void* Buffer::Map (GLenum access) {
  Bind();
  THROW_UPON_GL_ERROR(void *ptr = glMapBuffer(m_BufferType, access));
  Unbind();
  return ptr;
}

bool Buffer::Unmap () {
  Bind();
  THROW_UPON_GL_ERROR(bool result = glUnmapBuffer(m_BufferType) == GL_TRUE);
  Unbind();
  return result;
}

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
