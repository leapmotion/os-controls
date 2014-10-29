#pragma once

#include "GLShader.h"
#include "ScopeGuard.h"

namespace Leap {
namespace GL {

inline void GLShaderBind (const GLShader &shader) { shader.Bind(); }
inline void GLShaderUnbind (const GLShader &shader) { shader.Unbind(); }

// Convenience typedef for a ScopeGuard type that will Bind and Unbind a GLShader object.
typedef ScopeGuard<GLShader,GLShaderBind,GLShaderUnbind> GLShaderBindingScopeGuard;

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
