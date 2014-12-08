#pragma once

#include "Leap/GL/ScopeGuard.h"
#include "Leap/GL/Shader.h"

namespace Leap {
namespace GL {

inline void GLShaderBind (const Shader &shader) { shader.Bind(); }
inline void GLShaderUnbind (const Shader &shader) { shader.Unbind(); }

// Convenience typedef for a ScopeGuard type that will Bind and Unbind a Shader object.
typedef ScopeGuard<Shader,GLShaderBind,GLShaderUnbind> GLShaderBindingScopeGuard;

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
