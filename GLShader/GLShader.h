#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "gl_glext_glu.h" // convenience header for cross-platform GL includes

// helper metafunction for simplifying the uniform modifiers
template <typename GLType_, size_t COMPONENT_COUNT_> struct UniformFunction { static const bool exists = false; };

// helper metafunction for simplifying the uniform matrix modifiers
template <size_t ROWS_, size_t COLUMNS_> struct UniformMatrixFunction { static const bool exists = false; };

enum MatrixStorageConvention { COLUMN_MAJOR, ROW_MAJOR };

// This class wraps compiling and binding GLSL shaders.  It was initially taken from Jerry Coffin's
// answer at http://stackoverflow.com/questions/2795044/easy-framework-for-opengl-shaders-in-c-c
class GLShader {
public:

  // TODO: make GLShader-specific std::exception subclass?

  // Construct a shader with given vertex and fragment programs.
  GLShader (const std::string &vertex_shader_source, const std::string &fragment_shader_source);
  // Automatically frees the allocated resources.
  ~GLShader ();

  // This method should be called to bind this shader.
  void Bind () { glUseProgram(m_prog); }
  // This method should be called when no shader program should be used.
  static void Unbind () { glUseProgram(0); }

  // Returns true iff the shader uniform exists.
  bool HasUniform (const std::string &name) const { return LocationOfUniform(name) != -1; }
  // Returns true iff the shader attribute exists.
  bool HasAttribute (const std::string &name) const { return LocationOfAttribute(name) != -1; }
  // Returns the location of the requested uniform -- it's handle into the GL apparatus.
  GLint LocationOfUniform (const std::string &name) const {
    // TODO: cache these in a dictionary
    // TODO: throw if the name is not found?
    // QUESTION: there are two shaders -- fragment and vertex, each of which could define the same
    // uniform name using different types.  is this a problem, or does the shader compiler/linker
    // check this?
    return glGetUniformLocation(m_prog, name.c_str());
  }
  // Returns the location of the requested attribute -- it's handle into the GL apparatus.
  GLint LocationOfAttribute (const std::string &name) const {
    // TODO: cache these in a dictionary
    // TODO: throw if the name is not found?
    // QUESTION: there are two shaders -- fragment and vertex, each of which could define the same
    // uniform name using different types.  is this a problem, or does the shader compiler/linker
    // check this?
    return glGetAttribLocation(m_prog, name.c_str());
  }

  // These SetUniform* methods require this shader to currently be bound.  They are named
  // with type annotators to avoid confusion in situations where types are implicitly coerced.
  // The uniform has a fixed type in the shader, so the call to SetUniform* should reflect that.

  // Sets the named uniform to the given bool value (casted to GLint)
  void SetUniformi (const std::string &name, bool value) {
    SetUniformi(name, GLint(value));
  }
  // Sets the named uniform to the given GLint value.
  void SetUniformi (const std::string &name, GLint value) {
    glUniform1i(LocationOfUniform(name), value);
  }
  // Sets the named uniform to the given GLfloat value.
  void SetUniformf (const std::string &name, GLfloat value) {
    glUniform1f(LocationOfUniform(name), value);
  }
  // Sets the named uniform to the given value which must be a packed 
  // POD type consisting of exactly 1, 2, 3, or 4 GLint values.
  template <typename T_>
  void SetUniformi (const std::string &name, const T_ &value) {
    static_assert(sizeof(T_)%sizeof(GLint) == 0, "sizeof(T_) must be divisible by sizeof(GLint)");
    static_assert(UniformFunction<GLint,sizeof(T_)/sizeof(GLint)>::exists, "There is no known glUniform*i function for size of given T_");
    // TODO: somehow check that T_ is actually a POD containing only GLint components.
    UniformFunction<GLint,sizeof(T_)/sizeof(GLint)>::eval(LocationOfUniform(name), 1, reinterpret_cast<const GLint *>(&value));
  }
  // Sets the named uniform to the given value which must be a packed 
  // POD type consisting of exactly 1, 2, 3, or 4 GLfloat values.
  template <typename T_>
  void SetUniformf (const std::string &name, const T_ &value) {
    static_assert(sizeof(T_)%sizeof(GLfloat) == 0, "sizeof(T_) must be divisible by sizeof(GLfloat)");
    static_assert(UniformFunction<GLfloat,sizeof(T_)/sizeof(GLfloat)>::exists, "There is no known glUniform*i function for size of given T_");
    // TODO: somehow check that T_ is actually a POD containing only GLfloat components.
    UniformFunction<GLfloat,sizeof(T_)/sizeof(GLfloat)>::eval(LocationOfUniform(name), 1, reinterpret_cast<const GLfloat *>(&value));
  }

  // Uniform modifiers to specify arrays of data, where the array component has various
  // sizes (indicated by the number in the method name).

  // Sets the named uniform to the given std::vector of GLint values.
  void SetUniformi (const std::string &name, const std::vector<GLint> &array) {
    glUniform1iv(LocationOfUniform(name), array.size(), array.data());
  }
  // Sets the named uniform to the given std::vector of GLfloat values.
  void SetUniformf (const std::string &name, const std::vector<GLfloat> &array) {
    glUniform1fv(LocationOfUniform(name), array.size(), array.data());
  }
  // Sets the named uniform to the given std::vector of values each of which must be
  // a packed POD type consisting of exactly 1, 2, 3, or 4 GLint values.
  template <typename T_>
  void SetUniformi (const std::string &name, const std::vector<T_> &array) {
    static_assert(sizeof(T_)%sizeof(GLint) == 0, "sizeof(T_) must be divisible by sizeof(GLint)");
    static_assert(UniformFunction<GLint,sizeof(T_)/sizeof(GLint)>::exists, "There is no known glUniform*iv function for size of given T_");
    // TODO: somehow check that T_ is actually a POD containing only GLint components.
    UniformFunction<GLint,sizeof(T_)/sizeof(GLint)>::eval(LocationOfUniform(name), array.size(), reinterpret_cast<const GLint *>(array.data()));
  }
  // Sets the named uniform to the given std::vector of values each of which must be
  // a packed POD type consisting of exactly 1, 2, 3, or 4 GLfloat values.
  template <typename T_>
  void SetUniformf (const std::string &name, const std::vector<T_> &array) {
    static_assert(sizeof(T_)%sizeof(GLfloat) == 0, "sizeof(T_) must be divisible by sizeof(GLfloat)");
    static_assert(UniformFunction<GLfloat,sizeof(T_)/sizeof(GLfloat)>::exists, "There is no known glUniform*i function for size of given T_");
    // TODO: somehow check that T_ is actually a POD containing only GLfloat components.
    UniformFunction<GLfloat,sizeof(T_)/sizeof(GLfloat)>::eval(LocationOfUniform(name), array.size(), reinterpret_cast<const GLfloat *>(array.data()));
  }

  // Note that there are no GLint-based matrix-uniform-setting functions (because there are none in the GLSL spec)

  // Sets the named uniform to the given value which must be a packed POD type
  // consisting of exactly ROWS_*COLUMNS_ GLfloat values.  The matrix storage
  // convention must be specified; either ROW_MAJOR or COLUMN_MAJOR.
  template <size_t ROWS_, size_t COLUMNS_, typename T_>
  void SetUniformMatrixf (const std::string &name, const T_ &matrix, MatrixStorageConvention matrix_storage_convention) {
    static_assert(UniformMatrixFunction<ROWS_,COLUMNS_>::exists, "There is no glUniformMatrix* function matching the requested ROWS_ and COLUMNS_");
    static_assert(sizeof(T_) == ROWS_*COLUMNS_*sizeof(GLfloat), "T_ must be a POD type having exactly ROWS_*COLUMNS_ components of type GLfloat");
    // TODO: somehow check that T_ is actually a POD containing only GLType_ components.
    UniformMatrixFunction<ROWS_,COLUMNS_>::eval(LocationOfUniform(name), 1, matrix_storage_convention == ROW_MAJOR, reinterpret_cast<const GLfloat *>(&matrix));
  }
  // Sets the named uniform to the given std::vector of values each of which must be
  // a packed POD type consisting of exactly ROWS_*COLUMNS_ GLfloat values.  The matrix
  // storage convention must be specified; either ROW_MAJOR or COLUMN_MAJOR.
  template <size_t ROWS_, size_t COLUMNS_, typename T_>
  void SetUniformMatrixf (const std::string &name, const std::vector<T_> &array, MatrixStorageConvention matrix_storage_convention) {
    static_assert(UniformMatrixFunction<ROWS_,COLUMNS_>::exists, "There is no glUniformMatrix* function matching the requested ROWS_ and COLUMNS_");
    static_assert(sizeof(T_) == ROWS_*COLUMNS_*sizeof(GLfloat), "T_ must be a POD type having exactly ROWS_*COLUMNS_ components of type GLfloat");
    // TODO: somehow check that T_ is actually a POD containing only GLType_ components.
    UniformMatrixFunction<ROWS_,COLUMNS_>::eval(LocationOfUniform(name), array.size(), matrix_storage_convention == ROW_MAJOR, reinterpret_cast<const GLfloat *>(array.data()));
  }

private:

  // Compiles the specified type of shader program, using the given source.  If an error
  // in encountered, a std::logic_error is thrown.
  static GLuint Compile (GLuint type, const std::string &source);

  GLuint m_vertex_shader;   ///< Handle to the vertex shader in the GL apparatus.
  GLuint m_fragment_shader; ///< Handle to the fragment shader in the GL apparatus.
  GLuint m_prog;            ///< Handle to the shader program in the GL apparatus.
};

// Template specializations of UniformFunction and UniformMatrixFunction.
// TODO: Try to consolidate these into fewer specializations (or none)

/// @cond false
// we don't want these showing up in the class list.
template <> struct UniformFunction<GLint,1> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLint *value) { glUniform1iv(location, count, value); } };
template <> struct UniformFunction<GLint,2> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLint *value) { glUniform2iv(location, count, value); } };
template <> struct UniformFunction<GLint,3> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLint *value) { glUniform3iv(location, count, value); } };
template <> struct UniformFunction<GLint,4> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLint *value) { glUniform4iv(location, count, value); } };
template <> struct UniformFunction<GLfloat,1> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLfloat *value) { glUniform1fv(location, count, value); } };
template <> struct UniformFunction<GLfloat,2> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLfloat *value) { glUniform2fv(location, count, value); } };
template <> struct UniformFunction<GLfloat,3> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLfloat *value) { glUniform3fv(location, count, value); } };
template <> struct UniformFunction<GLfloat,4> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLfloat *value) { glUniform4fv(location, count, value); } };

template <> struct UniformMatrixFunction<2,2> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix2fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<2,3> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix2x3fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<2,4> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix2x4fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<3,2> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix3x2fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<3,3> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix3fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<3,4> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix3x4fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<4,2> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix4x2fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<4,3> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix4x3fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<4,4> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix4fv(location, count, transpose, value); } };
/// @endcond
