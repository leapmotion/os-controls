#pragma once

#include "Leap/GL/ScopeGuard.h"
#include "Leap/GL/Shader.h"

namespace Leap {
namespace GL {

inline void ShaderBind (const Shader &shader) { shader.Bind(); }
inline void ShaderUnbind (const Shader &shader) { shader.Unbind(); }

// Convenience typedef for a ScopeGuard type that will Bind and Unbind a Shader object.
typedef ScopeGuard<Shader,ShaderBind,ShaderUnbind> ShaderBindingScopeGuard;

} // end of namespace GL
} // end of namespace Leap
