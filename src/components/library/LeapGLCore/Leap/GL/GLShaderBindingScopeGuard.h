#pragma once

#include "GLShader.h"
#include "ScopeGuard.h"

inline void GLShaderBind (const GLShader &shader) { shader.Bind(); }
inline void GLShaderUnbind (const GLShader &shader) { shader.Unbind(); }

// Convenience typedef for a ScopeGuard type that will Bind and Unbind a GLShader object.
typedef ScopeGuard<GLShader,GLShaderBind,GLShaderUnbind> GLShaderBindingScopeGuard;
