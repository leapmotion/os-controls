#pragma once

#include "Leap/GL/Common.h"
#include "Leap/GL/GLHeaders.h" // convenience header for cross-platform GL includes
#include "Leap/GL/Internal/UniformSetterTraits.h"
#include "Leap/GL/ShaderException.h"

namespace Leap {
namespace GL {
// The contents of the Internal namespace are not intended to be used publicly, and provide
// no guarantee as to the stability of their API.  The classes and functions are used
// internally in the implementation of the publicly-presented classes.
namespace Internal {

template <GLenum GL_TYPE_> struct UniformSetter;

// std::is_fundamental is used so that the Set function which accepts `const T_ &` is disabled
// for argument types such as GLfloat, GLboolean, etc.  Otherwise the wrong overload is called
// for UniformSetter<GL_BOOL>::Set(loc, true).

#define DEFINE_UNIFORM_SETTER(GL_TYPE) \
  template <> struct UniformSetter<GL_TYPE> { \
    static_assert(UniformSetterTraits<GL_TYPE>::IS_DEFINED, "UniformSetterTraits<GL_TYPE> not defined."); \
    template <typename... Types_> \
    static void Set (GLint location, Types_... args) { \
      UniformSetterTraits<GL_TYPE>::SetUsingValues(location, args...); \
    } \
    template <typename T_> \
    static typename std::enable_if<!std::is_fundamental<T_>::value>::type Set (GLint location, const T_ &value) { \
      typedef typename UniformSetterTraits<GL_TYPE>::UniformArgumentType UniformArgumentType; \
      UniformSetterTraits<GL_TYPE>::CheckCompatibilityOf<T_,1>(); \
      UniformSetterTraits<GL_TYPE>::SetUsingPointer(location, 1, reinterpret_cast<const UniformArgumentType *>(&value)); \
    } \
    template <size_t ARRAY_LENGTH_, typename T_> \
    static void SetArray (GLint location, const T_ &value) { \
      typedef typename UniformSetterTraits<GL_TYPE>::UniformArgumentType UniformArgumentType; \
      UniformSetterTraits<GL_TYPE>::CheckCompatibilityOf<T_,ARRAY_LENGTH_>(); \
      UniformSetterTraits<GL_TYPE>::SetUsingPointer(location, ARRAY_LENGTH_, reinterpret_cast<const UniformArgumentType *>(&value)); \
    } \
  }

#define DEFINE_MATRIX_UNIFORM_SETTER(GL_TYPE) \
  template <> struct UniformSetter<GL_TYPE> { \
    static_assert(UniformSetterTraits<GL_TYPE>::IS_DEFINED, "UniformSetterTraits<GL_TYPE> not defined."); \
    template <typename T_> \
    static void Set (GLint location, const T_ &value, MatrixStorageConvention matrix_storage_convention) { \
      typedef typename UniformSetterTraits<GL_TYPE>::UniformArgumentType UniformArgumentType; \
      UniformSetterTraits<GL_TYPE>::CheckCompatibilityOf<T_,1>(); \
      UniformSetterTraits<GL_TYPE>::SetUsingPointer(location, 1, matrix_storage_convention, reinterpret_cast<const UniformArgumentType *>(&value)); \
    } \
    template <size_t ARRAY_LENGTH_, typename T_> \
    static void SetArray (GLint location, const T_ &value, MatrixStorageConvention matrix_storage_convention) { \
      typedef typename UniformSetterTraits<GL_TYPE>::UniformArgumentType UniformArgumentType; \
      UniformSetterTraits<GL_TYPE>::CheckCompatibilityOf<T_,ARRAY_LENGTH_>(); \
      UniformSetterTraits<GL_TYPE>::SetUsingPointer(location, ARRAY_LENGTH_, matrix_storage_convention, reinterpret_cast<const UniformArgumentType *>(&value)); \
    } \
  }

// OpenGL 2.1 uniform types

DEFINE_UNIFORM_SETTER(GL_BOOL);
DEFINE_UNIFORM_SETTER(GL_BOOL_VEC2);
DEFINE_UNIFORM_SETTER(GL_BOOL_VEC3);
DEFINE_UNIFORM_SETTER(GL_BOOL_VEC4);
DEFINE_UNIFORM_SETTER(GL_FLOAT);
DEFINE_UNIFORM_SETTER(GL_FLOAT_VEC2);
DEFINE_UNIFORM_SETTER(GL_FLOAT_VEC3);
DEFINE_UNIFORM_SETTER(GL_FLOAT_VEC4);
DEFINE_UNIFORM_SETTER(GL_INT);
DEFINE_UNIFORM_SETTER(GL_INT_VEC2);
DEFINE_UNIFORM_SETTER(GL_INT_VEC3);
DEFINE_UNIFORM_SETTER(GL_INT_VEC4);

DEFINE_MATRIX_UNIFORM_SETTER(GL_FLOAT_MAT2);
DEFINE_MATRIX_UNIFORM_SETTER(GL_FLOAT_MAT2x3);
DEFINE_MATRIX_UNIFORM_SETTER(GL_FLOAT_MAT2x4);
DEFINE_MATRIX_UNIFORM_SETTER(GL_FLOAT_MAT3x2);
DEFINE_MATRIX_UNIFORM_SETTER(GL_FLOAT_MAT3);
DEFINE_MATRIX_UNIFORM_SETTER(GL_FLOAT_MAT3x4);
DEFINE_MATRIX_UNIFORM_SETTER(GL_FLOAT_MAT4x2);
DEFINE_MATRIX_UNIFORM_SETTER(GL_FLOAT_MAT4x3);
DEFINE_MATRIX_UNIFORM_SETTER(GL_FLOAT_MAT4);

DEFINE_UNIFORM_SETTER(GL_SAMPLER_1D);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_2D);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_3D);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_CUBE);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_1D_SHADOW);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_2D_SHADOW);

// OpenGL 3.3 uniform types

DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_VEC2);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_VEC3);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_VEC4);

DEFINE_UNIFORM_SETTER(GL_SAMPLER_1D_ARRAY);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_2D_ARRAY);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_1D_ARRAY_SHADOW);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_2D_ARRAY_SHADOW);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_2D_MULTISAMPLE);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_2D_MULTISAMPLE_ARRAY);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_CUBE_SHADOW);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_BUFFER);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_2D_RECT);
DEFINE_UNIFORM_SETTER(GL_SAMPLER_2D_RECT_SHADOW);
DEFINE_UNIFORM_SETTER(GL_INT_SAMPLER_1D);
DEFINE_UNIFORM_SETTER(GL_INT_SAMPLER_2D);
DEFINE_UNIFORM_SETTER(GL_INT_SAMPLER_3D);
DEFINE_UNIFORM_SETTER(GL_INT_SAMPLER_CUBE);
DEFINE_UNIFORM_SETTER(GL_INT_SAMPLER_1D_ARRAY);
DEFINE_UNIFORM_SETTER(GL_INT_SAMPLER_2D_ARRAY);
DEFINE_UNIFORM_SETTER(GL_INT_SAMPLER_2D_MULTISAMPLE);
DEFINE_UNIFORM_SETTER(GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY);
DEFINE_UNIFORM_SETTER(GL_INT_SAMPLER_BUFFER);
DEFINE_UNIFORM_SETTER(GL_INT_SAMPLER_2D_RECT);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_SAMPLER_1D);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_SAMPLER_2D);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_SAMPLER_3D);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_SAMPLER_CUBE);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_SAMPLER_1D_ARRAY);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_SAMPLER_BUFFER);
DEFINE_UNIFORM_SETTER(GL_UNSIGNED_INT_SAMPLER_2D_RECT);

} // end of namespace Internal
} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
