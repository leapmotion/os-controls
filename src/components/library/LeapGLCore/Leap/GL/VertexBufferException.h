#pragma once

#include "Leap/GL/Exception.h"

namespace Leap {
namespace GL {

// Base class for all Leap::GL::VertexBuffer exceptions.
class VertexBufferException : public Exception {
public:

  VertexBufferException (const std::string &message) : Exception(message) { }
};

} // end of namespace GL
} // end of namespace Leap
