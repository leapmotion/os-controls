#include "GLController.h"

#include <iostream> // TEMP

GLController::GLController () {

}

GLController::~GLController () {

}

void GLController::Initialize () {
  std::cerr << "GL_VERSION = \"" << GetString(GL_VERSION) << "\"\n";       // TEMP
  std::cerr << "GL_RENDERER = \"" << GetString(GL_RENDERER) << "\"\n";     // TEMP
  std::cerr << "GL_VENDOR = \"" << GetString(GL_VENDOR) << "\"\n";         // TEMP
  // std::cerr << "GL_EXTENSIONS = \"" << GetString(GL_EXTENSIONS) << "\"\n"; // TEMP
}

void GLController::Shutdown () {

}

void GLController::BeginRender () const {

}

void GLController::EndRender () const {
  glFlush();
}

std::string GLController::GetString (GLenum name) {
  // TODO: throw exception upon error?
  return std::string(reinterpret_cast<const char *>(glGetString(name)));
}

/*
Design brainstorming for GLController methods
------------------------------------------
The goal is to provide a minimal but clear C++ interface to the GL state, caching 
certain state variables with the purpose of minimizing number of GL calls.  A limited 
set of function calls control the state, and some functions' effects depend on particular 
state variables (e.g. glUniform*, controlling values for the currently loaded shader program).

NOTE: One of the goals of the code created by this Components team is to create code that
is highly modularized, having minimal dependency.  If a full state-tracking GL class were made,
it would essentially be incompatible with anything that does "raw" GL calls, since this would
modify the GL state without the knowledge of the C++ GL state object.

If it were possible to "reroute" all GL calls through "our" functions (the corresponding methods
called on the GL state singleton object), then no code would need to be changed for such a
facility to be used, including that of library code, which should be considered a design
criteria.  The advantage is then we would in principle be aware of and control all GL calls.
The disadvantage is that what appear to be "plain old C function calls" then gain this new,
mysterious, and possibly unexpected behavior.

Some relevant info:

Wrapping symbols via a linker step:
http://stackoverflow.com/questions/13961774/gnu-gcc-ld-wrapping-a-call-to-symbol-with-caller-and-callee-defined-in-the-sam

This technique could be used in other ways, such as replacing malloc/calloc/free.

Here is a list of all OpenGL 3.3 functions, as listed by http://www.opengl.org/sdk/docs/man3/

Annotations have been made, from reading the docs for each function, to the following effect:
- If the function's behavior is independent of other GL state (marked with Ind), or the
  opposite (Dep).
- The "Look" annotation indicates that the function is interesting and should be considered more.

The purpose of the annotations is to discover which functions' state can be cached trivially,
and which functions' state must be cached in a way that requires more information (i.e. changing
the value of one state variable would require clearing the cache on another).

A
glActiveTexture                       Ind
glAttachShader                        Dep

B
glBeginConditionalRender
glBeginQuery
glBeginTransformFeedback
glBindAttribLocation
glBindBuffer                          Dep
glBindBufferBase                      Dep
glBindBufferRange                     Dep
glBindFragDataLocation
glBindFragDataLocationIndexed
glBindFramebuffer                     Dep
glBindRenderbuffer                    Dep
glBindSampler                         Dep
glBindTexture                         Dep
glBindVertexArray                     Dep
glBlendColor                          Ind
glBlendEquation                       Ind
glBlendEquationSeparate               Ind
glBlendFunc                           Ind
glBlendFuncSeparate                   
glBlitFramebuffer
glBufferData                          Look
glBufferSubData                       Look

C
glCheckFramebufferStatus
glClampColor                          Ind
glClear                               Dep
glClearBuffer                         Dep
glClearColor                          Ind
glClearDepth                          Ind
glClearStencil                        Ind
glClientWaitSync
glColorMask                           Ind
glColorMaski                          start here
glCompileShader
glCompressedTexImage1D
glCompressedTexImage2D
glCompressedTexImage3D
glCompressedTexSubImage1D
glCompressedTexSubImage2D
glCompressedTexSubImage3D
glCopyBufferSubData
glCopyTexImage1D
glCopyTexImage2D
glCopyTexSubImage1D
glCopyTexSubImage2D
glCopyTexSubImage3D
glCreateProgram
glCreateShader
glCullFace

D
glDeleteBuffers
glDeleteFramebuffers
glDeleteProgram
glDeleteQueries
glDeleteRenderbuffers
glDeleteSamplers
glDeleteShader
glDeleteSync
glDeleteTextures
glDeleteVertexArrays
glDepthFunc
glDepthMask
glDepthRange
glDetachShader
glDisable
glDisableVertexAttribArray
glDisablei
glDrawArrays
glDrawArraysInstanced
glDrawBuffer
glDrawBuffers
glDrawElements
glDrawElementsBaseVertex
glDrawElementsInstanced
glDrawElementsInstancedBaseVertex
glDrawRangeElements
glDrawRangeElementsBaseVertex

E
glEnable
glEnableVertexAttribArray
glEndConditionalRender
glEndQuery
glEndTransformFeedback

F
glFenceSync
glFinish
glFlush
glFlushMappedBufferRange
glFramebufferRenderbuffer
glFramebufferTexture
glFramebufferTextureLayer
glFrontFace

G
glGenBuffers
glGenFramebuffers
glGenQueries
glGenRenderbuffers
glGenSamplers
glGenTextures
glGenVertexArrays
glGenerateMipmap
glGet
glGetActiveAttrib
glGetActiveUniform
glGetActiveUniformBlock
glGetActiveUniformBlockName
glGetActiveUniformName
glGetActiveUniformsiv
glGetAttachedShaders
glGetAttribLocation
glGetBufferParameter
glGetBufferPointerv
glGetBufferSubData
glGetCompressedTexImage
glGetError
glGetFragDataIndex
glGetFragDataLocation
glGetFramebufferAttachmentParameter
glGetMultisample
glGetProgram
glGetProgramInfoLog
glGetQueryObject
glGetQueryiv
glGetRenderbufferParameter
glGetSamplerParameter
glGetShader
glGetShaderInfoLog
glGetShaderSource
glGetString
glGetSync
glGetTexImage
glGetTexLevelParameter
glGetTexParameter
glGetTransformFeedbackVarying
glGetUniform
glGetUniformBlockIndex
glGetUniformIndices
glGetUniformLocation
glGetVertexAttrib
glGetVertexAttribPointerv

H
glHint

I
glIsBuffer
glIsEnabled
glIsFramebuffer
glIsProgram
glIsQuery
glIsRenderbuffer
glIsSampler
glIsShader
glIsSync
glIsTexture
glIsVertexArray

L
glLineWidth
glLinkProgram
glLogicOp

M
glMapBuffer
glMapBufferRange
glMultiDrawArrays
glMultiDrawElements
glMultiDrawElementsBaseVertex

P
glPixelStore
glPointParameter
glPointSize
glPolygonMode
glPolygonOffset
glPrimitiveRestartIndex
glProvokingVertex

Q
glQueryCounter

R
glReadBuffer
glReadPixels
glRenderbufferStorage
glRenderbufferStorageMultisample

S
glSampleCoverage
glSampleMaski
glSamplerParameter
glScissor
glShaderSource
glStencilFunc
glStencilFuncSeparate
glStencilMask
glStencilMaskSeparate
glStencilOp
glStencilOpSeparate

T
glTexBuffer
glTexImage1D
glTexImage2D
glTexImage2DMultisample
glTexImage3D
glTexImage3DMultisample
glTexParameter
glTexSubImage1D
glTexSubImage2D
glTexSubImage3D
glTransformFeedbackVaryings

U
glUniform
glUniformBlockBinding
glUnmapBuffer
glUseProgram

V
glValidateProgram
glVertexAttrib
glVertexAttribDivisor
glVertexAttribPointer
glViewport

W
glWaitSync


*/
