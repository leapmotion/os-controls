#pragma once

#include "gl_glext_glu.h"
#include "GLBuffer.h"
#include "GLVertexAttribute.h"

#include <tuple>
#include <type_traits>
#include <vector>

// Design notes for GLVertexBuffer
// -------------------------------
// This will encapsulate the concept of an OpenGL vertex buffer -- this is a set of
// attributes for each vertex, such as position, normal, texture coordinate, color,
// and whatever else.  Each attribute must correspond exactly to one attribute in the
// associated vertex shader.  Each vertex in the buffer must have a well-defined value
// for each attribute.  This class will automatically handle creation, allocation, deallocation
// and destruction of the relevant resources (e.g. GL buffers, etc).
//
// The association between the attributes in a GLVertexBuffer and the attributes in
// a vertex shader is done by shader attribute name.  Instead of hard-coding the attribute
// names into GLVertexBuffer, the names of the associated attributes will be specified
// to GLVertexBuffer for the Enable operation.  This correspondence is essentially a
// dictionary of type attribute -> attribute location.  Some vertex shaders may not have
// all of the attributes that are provided by a given GLVertexBuffer, and that should be
// handled by ignoring the missing attributes.  Thus if a vertex shader only has e.g.
// the "position" attribute but a GLVertexBuffer has a position and normal attribute for
// each vertex, said GLVertexBuffer can be used with the shader, and the normal attribute
// will go unused.
//
// The attributes in a GLVertexBuffer should be fully configurable, of any type that is
// allowable by GL.  This should be done with templates, so that the vertex-specifying
// methods are fully strongly typed.  There should be a "vertex attribute" class which
// manages the data associated with each attribute and translates the C++-typed attributes
// into the relevant GL enums and data.


// Encapsulates the concept of an OpenGL vertex buffer object.  A vertex buffer
// object is an array of vertex attributes that are uploaded to the GPU for use
// in a vertex shader program.  The vertex attributes correspond to "attribute"
// variables in the vertex shader.  The vertex shader program is called on each
// vertex, and each vertex must have each of the required attributes defined
// for it.  Thus the set of attributes must be well-defined.  This is done via
// the variadic AttributeTypes template parameter(s).
//
// There is a vector of "intermediate attributes" which is a mutable buffer for
// creating/modifying vertex attributes before they are [re]uploaded to the GPU.
// Once the intermediate attributes vector has been populated, the
// UploadIntermediateAttributes should be called, which will create the necessary
// GL resource and upload the data to the GPU.  Unless the intermediate attributes
// are going to be modified and re-uploaded, it is recommended to clear the
// intermediate attributes after upload.
template <typename... AttributeTypes>
class GLVertexBuffer {
public:

  typedef std::tuple<AttributeTypes...> Attributes;
  static const size_t ATTRIBUTE_COUNT = std::tuple_size<Attributes>::value;

  // The usage_pattern parameter specifies the expected usage pattern of the data store.
  // It must be one of: GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW,
  // GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY.
  // See glBufferData for more on this.
  GLVertexBuffer (GLenum usage_pattern)
    :
    m_usage_pattern(usage_pattern)
  {
    switch (m_usage_pattern) {
      case GL_STREAM_DRAW:
      case GL_STREAM_READ:
      case GL_STREAM_COPY:
      case GL_STATIC_DRAW:
      case GL_STATIC_READ:
      case GL_STATIC_COPY:
      case GL_DYNAMIC_DRAW:
      case GL_DYNAMIC_READ:
      case GL_DYNAMIC_COPY:
        break; // Ok
      default:
        throw std::invalid_argument("usage must be one of GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY.");
    }
  }

  // For use in unique-ifying vertex attributes.
  struct AttributesCompare {
    bool operator () (const Attributes &lhs, const Attributes &rhs) const {
      return memcmp(reinterpret_cast<const void *>(&lhs), reinterpret_cast<const void *>(&rhs), sizeof(Attributes)) < 0;
    }
  };

  // Returns the usage pattern used in upload operations.
  GLenum UsagePattern () const { return m_usage_pattern; }
  // Returns a const reference to the vertex attributes' intermediate buffer that is used
  // to create/modify attributes before uploading to the GPU.
  const std::vector<Attributes> &IntermediateAttributes () const { return m_intermediate_attributes; }
  // Returns a reference to the vertex attributes' intermediate buffer that is used
  // to create/modify attributes before uploading to the GPU.
  std::vector<Attributes> &IntermediateAttributes () { return m_intermediate_attributes; }

  // This releases all resources (pre-load attribute buffer and GL buffer
  // for vertex attributes, if it has been created already).
  void ClearEverything () {
    ClearIntermediateAttributes();
    ClearGLResources();
  }
  // This clears the intermediate attribute buffer, but preserves everything else.
  void ClearIntermediateAttributes () {
    m_intermediate_attributes.clear();
  }
  // This clears the GL buffer object, but preserves everything else.
  void ClearGLResources () const {
    m_gl_buffer.Destroy();
  }
  // Allocates (if necessary) and populates a GL buffer object with the intermediate attribute
  // buffer data.  It is recommended to clear the intermediate attributes after calling this
  // method, unless said attribute data is going to be modified and uploaded again.  This
  // method should only be called after the intermediate attributes have changed and the
  // changes need to be propagated to the GPU.
  void UploadIntermediateAttributes () const {
    GLsizeiptr intermediate_attributes_size(m_intermediate_attributes.size()*sizeof(Attributes));
    const void *intermediate_attributes_data(m_intermediate_attributes.data());
    // If the buffer is already created and is the same size as the intermediate attributes,
    // then map it and copy the data in.
    if (m_gl_buffer.IsCreated() && m_gl_buffer.Size() == intermediate_attributes_size) {
      void *ptr = m_gl_buffer.Map(GL_WRITE_ONLY);
      memcpy(ptr, intermediate_attributes_data, intermediate_attributes_size);
      m_gl_buffer.Unmap();
    } else { // Otherwise ensure the buffer is created, 
      if (!m_gl_buffer.IsCreated()) {
        m_gl_buffer.Create(GL_ARRAY_BUFFER);
      }
      m_gl_buffer.Bind();
      // This will delete and reallocate if it's already allocated.
      m_gl_buffer.Allocate(intermediate_attributes_data, intermediate_attributes_size, m_usage_pattern);
      m_gl_buffer.Unbind();
    }
  }

  // This method calls glEnableVertexAttribArray and glVertexAttribPointer on each
  // of the vertex attributes given valid locations (i.e. not equal to -1).  The
  // tuple argument attribute_locations must correspond exactly to Attributes
  // (which is a tuple of GLVertexAttribute types defined by this GLVertexBuffer).
  template <typename... LocationTypes>
  void Enable (const std::tuple<LocationTypes...> &attribute_locations) const {
    if (!m_gl_buffer.IsCreated()) {
      throw std::runtime_error("can't Enable a GLVertexBuffer that hasn't had UploadIntermediateAttributes called on it");
    }
    m_gl_buffer.Bind();
    // Begin iterated binding of vertex attributes starting at the 0th one.
    EnableAndIterate<0>(attribute_locations, sizeof(Attributes), 0);
    m_gl_buffer.Unbind();
  }
  // This method calls glDisableVertexAttribArray on each of the vertex attributes
  // given valid locations (i.e. not equal to -1).  This method is analogous to the
  // Enable method.
  template <typename... LocationTypes>
  static void Disable (const std::tuple<LocationTypes...> &attribute_locations) {
    // Begin iterated unbinding of vertex attributes starting at the 0th one.
    DisableAndIterate<0>(attribute_locations);
  }

private:

  // This is one iteration of the Enable method.  It calls the next iteration.
  template <size_t INDEX, typename... LocationTypes>
  static typename std::enable_if<(INDEX<ATTRIBUTE_COUNT),void>::type
    EnableAndIterate (const std::tuple<LocationTypes...> &locations, size_t stride, size_t offset)
  {
    typedef std::tuple<LocationTypes...> Locations;
    static_assert(std::tuple_size<Locations>::value == ATTRIBUTE_COUNT, "Must specify the same number of locations as attributes");
    static_assert(std::is_same<typename std::tuple_element<INDEX,Locations>::type,GLint>::value, "May only specify GLint values for locations");
    // Get the INDEXth attribute type.
    typedef typename std::tuple_element<INDEX,Attributes>::type AttributeType;
    // Get the INDEXth location value.
    GLint location = std::get<INDEX>(locations);
    // Call the Enable method of the INDEXth attribute type with the INDEXth location value, etc.
    AttributeType::Enable(location, stride, offset);
    // Increment INDEX and call this method again (this is a meta-program for loop).
    EnableAndIterate<INDEX+1>(locations, stride, offset+sizeof(AttributeType));
  }
  // This is the end of the iteration in the Enable method.
  template <size_t INDEX, typename... LocationTypes>
  static typename std::enable_if<(INDEX>=ATTRIBUTE_COUNT),void>::type
    EnableAndIterate (const std::tuple<LocationTypes...> &locations, size_t stride, size_t offset)
  {
    // Iteration complete -- do nothing.
  }

  // This is one iteration of the Disable method.  It calls the next iteration.
  template <size_t INDEX, typename... LocationTypes>
  static typename std::enable_if<(INDEX<ATTRIBUTE_COUNT),void>::type
    DisableAndIterate (const std::tuple<LocationTypes...> &locations)
  {
    typedef std::tuple<LocationTypes...> Locations;
    static_assert(std::tuple_size<Locations>::value == ATTRIBUTE_COUNT, "Must specify the same number of locations as attributes");
    static_assert(std::is_same<typename std::tuple_element<INDEX,Locations>::type,GLint>::value, "May only specify GLint values for locations");
    // Get the INDEXth attribute type.
    typedef typename std::tuple_element<INDEX,Attributes>::type AttributeType;
    // Get the INDEXth location value.
    GLint location = std::get<INDEX>(locations);
    // Call the Disable method of the INDEXth attribute type with the INDEXth location value.
    AttributeType::Disable(location);
    // Increment INDEX and call this method again (this is a meta-program for loop).
    DisableAndIterate<INDEX+1>(locations);
  }
  // This is the end of the iteration in the Disable method.
  template <size_t INDEX, typename... LocationTypes>
  static typename std::enable_if<(INDEX>=ATTRIBUTE_COUNT),void>::type
    DisableAndIterate (const std::tuple<LocationTypes...> &locations)
  {
    // Iteration complete -- do nothing.
  }

  GLenum m_usage_pattern;
  std::vector<Attributes> m_intermediate_attributes;
  mutable GLBuffer m_gl_buffer;
};
