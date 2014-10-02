#pragma once

#include "gl_glext_glu.h"

// This is a metafunction which defines the component type and component count
// for the various shader attribute types.  TODO: figure out if there are others,
// e.g. samplers, that can be shader attributes.
template <GLenum ATTRIB_TYPE> struct GLVertexAttributeReflection;

#define GL_VERTEX_ATTRIBUTE_REFLECTION(ATTRIB_TYPE, ComponentType, COMPONENT_TYPE_ENUM, COMPONENT_COUNT) \
  template <> struct GLVertexAttributeReflection<ATTRIB_TYPE> { \
    typedef ComponentType Component; \
    static const GLenum TYPE_ENUM = COMPONENT_TYPE_ENUM; \
    static const size_t COUNT = COMPONENT_COUNT; \
  }

GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT,             GLfloat,   GL_FLOAT,        1);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT_VEC2,        GLfloat,   GL_FLOAT,        2);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT_VEC3,        GLfloat,   GL_FLOAT,        3);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT_VEC4,        GLfloat,   GL_FLOAT,        4);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_INT,               GLint,     GL_INT,          1);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_INT_VEC2,          GLint,     GL_INT,          2);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_INT_VEC3,          GLint,     GL_INT,          3);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_INT_VEC4,          GLint,     GL_INT,          4);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_UNSIGNED_INT,      GLuint,    GL_UNSIGNED_INT, 1);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_UNSIGNED_INT_VEC2, GLuint,    GL_UNSIGNED_INT, 2);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_UNSIGNED_INT_VEC3, GLuint,    GL_UNSIGNED_INT, 3);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_UNSIGNED_INT_VEC4, GLuint,    GL_UNSIGNED_INT, 4);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_BOOL,              GLboolean, GL_BOOL,         1);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_BOOL_VEC2,         GLboolean, GL_BOOL,         2);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_BOOL_VEC3,         GLboolean, GL_BOOL,         3);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_BOOL_VEC4,         GLboolean, GL_BOOL,         4);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT_MAT2,        GLfloat,   GL_FLOAT,      2*2);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT_MAT2x3,      GLfloat,   GL_FLOAT,      2*3);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT_MAT2x4,      GLfloat,   GL_FLOAT,      2*4);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT_MAT3x2,      GLfloat,   GL_FLOAT,      3*2);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT_MAT3,        GLfloat,   GL_FLOAT,      3*3);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT_MAT3x4,      GLfloat,   GL_FLOAT,      3*4);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT_MAT4x2,      GLfloat,   GL_FLOAT,      4*2);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT_MAT4x3,      GLfloat,   GL_FLOAT,      4*3);
GL_VERTEX_ATTRIBUTE_REFLECTION(GL_FLOAT_MAT4,        GLfloat,   GL_FLOAT,      4*4);

#undef GL_VERTEX_ATTRIBUTE_REFLECTION

// TODO: enabling use of the "normalized" boolean parameter in glVertexAttribPointer.
// This represents a single, typed attribute in a GLVertexBuffer.  It is a single
// array of fixed-typed components, defined via GLVertexAttributeReflection by the
// ATTRIB_TYPE template parameter.  Using the "As" methods, the array can be accessed
// and modified as whatever POD type (of correct size) is desired.
template <GLenum ATTRIB_TYPE>
class GLVertexAttribute {
public:

  typedef typename GLVertexAttributeReflection<ATTRIB_TYPE>::Component ComponentType;
  static const GLenum COMPONENT_TYPE_ENUM = GLVertexAttributeReflection<ATTRIB_TYPE>::TYPE_ENUM;
  static const size_t COMPONENT_COUNT = GLVertexAttributeReflection<ATTRIB_TYPE>::COUNT;

  GLVertexAttribute () { }
  template <typename T>
  GLVertexAttribute (const T &value) {
    As<T>() = value;
  }

  // This method is so that the components (a fixed-length array) can be accessed
  // as whatever POD type is desired (e.g. some POD library-specific vector type).
  template <typename T>
  const T &As () const {
    static_assert(sizeof(T) == COMPONENT_COUNT*sizeof(ComponentType), "T must be a POD mapping directly onto the component array");
    // TODO: somehow check that T is a POD consisting only of type Component
    return *reinterpret_cast<const T *>(&m_components[0]);
  }
  // This method is so that the components (a fixed-length array) can be accessed
  // as whatever POD type is desired (e.g. some POD library-specific vector type).
  template <typename T>
  T &As () {
    static_assert(sizeof(T) == COMPONENT_COUNT*sizeof(ComponentType), "T must be a POD mapping directly onto the component array");
    // TODO: somehow check that T is a POD consisting only of type Component
    return *reinterpret_cast<T *>(&m_components[0]);
  }

  // This method assumes that the vertex buffer that this attribute is associated
  // with is currently bound.  An attribute's use is optional.  Specifically, if
  // a particular vertex shader doesn't have a particular attribute present in a
  // given GLVertexBuffer, that GLVertexBuffer can still be used -- the missing
  // attributes will be ignored.  Specifying -1 for location indicates that this
  // attribute should not be used.  The calls to glVertexAttribPointer use the
  // currently bound buffer object.
  static void Enable (GLint location, GLsizei stride, GLsizei offset) {
    // NOTE: There being no 
    //   "else { glDisableVertexAttribArray(...); }"
    // statement relies on each vertex attrib array being disabled to begin with.
    if (location != -1) {
      glEnableVertexAttribArray(location);
      glVertexAttribPointer(location, COMPONENT_COUNT, COMPONENT_TYPE_ENUM, GL_FALSE, stride, reinterpret_cast<void*>(offset));
    }
  }
  // Opposite of the Enable method.  Again, only does anything if location is not -1.
  static void Disable (GLint location) {
    if (location != -1) {
      glDisableVertexAttribArray(location);
    }
  }

private:

  ComponentType m_components[COMPONENT_COUNT];
};
