#include "Leap/GL/BufferObject.h"

#include <cassert>
#include "Leap/GL/Error.h"

namespace Leap {
namespace GL {

BufferObject::BufferObject ()
  : m_BufferAddress(0)
  , m_BufferType(0)
  , m_SizeInBytes(0)
{ }

BufferObject::BufferObject (GLenum type)
  : m_BufferAddress(0)
  , m_BufferType(0)
  , m_SizeInBytes(0)
{
  Initialize(type);
}

BufferObject::~BufferObject () {
  Shutdown();
}

void BufferObject::Initialize (GLenum type) {
  // Ensure that any previously allocated resources are freed.
  Shutdown();
  // Set up the new buffer type.
  m_BufferType = type;
  THROW_UPON_GL_ERROR(glGenBuffers(1, &m_BufferAddress));
}

void BufferObject::Shutdown () {
  if (IsInitialized()) {
    // THROW_UPON_GL_ERROR(glDeleteBuffers(1, &m_BufferAddress));
    glDeleteBuffers(1, &m_BufferAddress);
    m_BufferAddress = 0;
    m_SizeInBytes = 0;
    assert(!IsInitialized());
  }
}

void BufferObject::Bind () const {
  if (!IsInitialized()) {
    throw Leap::GL::Exception("Can't call BufferObject::Bind on a BufferObject that is !IsInitialized().");
  }
  THROW_UPON_GL_ERROR(glBindBuffer(m_BufferType, m_BufferAddress));
}

void BufferObject::Unbind () const {
  if (!IsInitialized()) {
    throw Leap::GL::Exception("Can't call BufferObject::Unbind on a BufferObject that is !IsInitialized().");
  }
  THROW_UPON_GL_ERROR(glBindBuffer(m_BufferType, 0));
}

void BufferObject::Allocate (const void* data, GLsizeiptr size_in_bytes, GLenum usage_pattern) {
  if (!IsInitialized()) {
    throw Leap::GL::Exception("Can't call BufferObject::Allocate on a BufferObject that is !IsInitialized().");
  }
  THROW_UPON_GL_ERROR(glBufferData(m_BufferType, size_in_bytes, data, usage_pattern));
  m_SizeInBytes = size_in_bytes;
}

void BufferObject::Write (const void* data, int count) {
  if (!IsInitialized()) {
    throw Leap::GL::Exception("Can't call BufferObject::Write on a BufferObject that is !IsInitialized().");
  }
  THROW_UPON_GL_ERROR(glBufferSubData(m_BufferType, 0, count, data));
}

void* BufferObject::Map (GLenum access) {
  if (!IsInitialized()) {
    throw Leap::GL::Exception("Can't call BufferObject::Map on a BufferObject that is !IsInitialized().");
  }
  Bind();
  THROW_UPON_GL_ERROR(void *ptr = glMapBuffer(m_BufferType, access));
  Unbind();
  return ptr;
}

bool BufferObject::Unmap () {
  if (!IsInitialized()) {
    throw Leap::GL::Exception("Can't call BufferObject::Unmap on a BufferObject that is !IsInitialized().");
  }
  Bind();
  THROW_UPON_GL_ERROR(bool result = glUnmapBuffer(m_BufferType) == GL_TRUE);
  Unbind();
  return result;
}

} // end of namespace GL
} // end of namespace Leap
