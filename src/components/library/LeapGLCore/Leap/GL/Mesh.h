#pragma once

#include <cstdint>
#include "Leap/GL/Buffer.h"
#include "Leap/GL/VertexBuffer.h"
#include "Leap/GL/MeshException.h"
#include <map>
#include <vector>

namespace Leap {
namespace GL {

enum class ClearOption { KEEP_INTERMEDIATE_DATA, CLEAR_INTERMEDIATE_DATA };

// A useful resource on what the possible draw modes are is http://www.lighthouse3d.com/tutorials/glsl-core-tutorial/primitive-assembly/
template <typename... AttributeTypes>
class Mesh {
public:

  typedef VertexBuffer<AttributeTypes...> VertexBuffer;
  typedef typename VertexBuffer::Attributes VertexAttributes;

  // Constructs an empty mesh having no OpenGL resource usage or draw mode.  At the very least,
  // SetDrawMode and Initialize must be called for the resource to be acquired.
  Mesh ()
    : m_draw_mode(GL_INVALID_ENUM) // Uninitialized.
    , m_vertex_buffer(GL_STATIC_DRAW)
    , m_index_count(0)
  { }
  // While each of the members will handle their own cleanup in their destructors, it's good to have
  // uniform semantics for all the resource types.
  ~Mesh () { Release(); }

  GLenum DrawMode () const { return m_draw_mode; }
  // Sets the draw mode.  This method can't be called between Initialize and Release, or
  // an exception will be thrown.
  void SetDrawMode (GLenum draw_mode) {
    if (IsInitialized()) {
      throw MeshException("Can't call Mesh::SetDrawMode on a Mesh that IsInitialized.");
    }
    if (!m_intermediate_vertices.empty()) {
      throw MeshException("Changing the draw mode on an existing vertex array is not a well-defined operation.");
    }
    switch (draw_mode) {
      case GL_POINTS:
      case GL_LINE_STRIP:
      case GL_LINE_LOOP:
      case GL_LINES:
      case GL_LINE_STRIP_ADJACENCY:
      case GL_LINES_ADJACENCY:
      case GL_TRIANGLE_STRIP:
      case GL_TRIANGLE_FAN:
      case GL_TRIANGLES:
      case GL_TRIANGLE_STRIP_ADJACENCY:
      case GL_TRIANGLES_ADJACENCY:
        m_draw_mode = draw_mode;
        break;
      default:
        throw MeshException("Invalid draw mode -- must be one of GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY (see OpenGL 3.3 docs for glDrawElements).");
    }
  }

  bool IsInitialized () const {
    assert(m_vertex_buffer.IsInitialized() == m_index_buffer.IsCreated() && "m_vertex_buffer and m_index_buffer should either be IsInitialized or not IsInitialized at the same time.");
    return m_vertex_buffer.IsInitialized();
  }
  // Once all the vertices are specified, calling this method computes the vertex index buffer
  // and uploads the unique-ified vertices and the indices to the GPU.  If successful, the mesh
  // is ready to be Bind()ed.
  // TODO: write about what GL operations actually happen.
  // TODO: add argument to specify if the non-GL internal state should be cleared.
  void Initialize (ClearOption clear_option = ClearOption::CLEAR_INTERMEDIATE_DATA) {
    // TODO: move into the function which uses this.
    struct VertexAttributesCompare {
      bool operator () (const VertexAttributes &lhs, const VertexAttributes &rhs) const {
        return memcmp(reinterpret_cast<const void *>(&lhs), reinterpret_cast<const void *>(&rhs), sizeof(VertexAttributes)) < 0;
      }
    };
    typedef std::map<VertexAttributes,GLuint,VertexAttributesCompare> VertexIndexMap;

    // Eliminate duplicate vertices using a temporary map.
    VertexIndexMap vertex_attributes_index_map;
    std::vector<GLuint> indices;
    // std::vector<VertexAttributes> unique_vertex_attributes;
    std::vector<VertexAttributes> &unique_vertex_attributes = m_vertex_buffer.IntermediateAttributes();
    // This loop will reduce m_intermediate_vertices down into unique_vertex_attributes.
    for (auto it = m_intermediate_vertices.begin(); it != m_intermediate_vertices.end(); ++it) {
      const VertexAttributes &vertex_attributes = *it;
      auto mapped_vertex_attribute = vertex_attributes_index_map.find(vertex_attributes);
      // If the current vertex is not in the vbo map already, add it.
      if (mapped_vertex_attribute == vertex_attributes_index_map.end()) {
        GLuint new_index = static_cast<GLuint>(vertex_attributes_index_map.size());
        indices.push_back(new_index);
        unique_vertex_attributes.push_back(vertex_attributes);
        vertex_attributes_index_map[vertex_attributes] = new_index;
      } else { // Otherwise, add the existing vertex's index.
        indices.push_back(mapped_vertex_attribute->second);
      }
    }

    if (clear_option == ClearOption::CLEAR_INTERMEDIATE_DATA) {
      m_intermediate_vertices.clear();
    }

    m_vertex_buffer.UploadIntermediateAttributes();

    m_index_count = static_cast<int>(indices.size());

    m_index_buffer.Create(GL_ELEMENT_ARRAY_BUFFER);
    m_index_buffer.Bind();
    // TODO: allow different usage patterns
    m_index_buffer.Allocate(static_cast<void*>(indices.data()), static_cast<int>(indices.size()*sizeof(unsigned int)), GL_STATIC_DRAW);
    m_index_buffer.Unbind();

    // For now, always clear the intermediate attributes, because the unique'ified vertices and index buffer will
    // just be regenerated.  TODO: for meshes that just change attributes without changing indices, keeping this
    // around could make it easier to regen.
    m_vertex_buffer.ClearIntermediateAttributes();
  }
  // Releases any OpenGL resources used by this mesh.  The draw mode is preserved.
  // TODO: write about what GL operations actually happen.
  // TODO: add argument to specify if the rest of the internal state should be cleared.
  void Release (ClearOption clear_option = ClearOption::CLEAR_INTERMEDIATE_DATA) {
    if (clear_option == ClearOption::CLEAR_INTERMEDIATE_DATA) {
      m_intermediate_vertices.clear();
    }
    m_vertex_buffer.ClearEverything();
    m_index_count = 0;
    if (m_index_buffer.IsCreated()) {
      m_index_buffer.Destroy();
    }
  }

  // TODO: think about if these functions really belong here, or if the user should call the VBO and
  // index buffer bind methods and the draw methods themselves.

  // Binds this mesh (vertex buffer, index buffer) so that it can be rendered via glDrawElements.
  // Specifying -1 for any individual attribute location will cause that attribute to go unused --
  // this allows shaders that don't have all the attributes in this Mesh to still be usable.
  // TODO: write about what GL operations actually happen.
  void Bind (typename VertexBuffer::UniformLocations &attribute_locations) const {
    if (!IsInitialized()) {
      throw MeshException("Can't Bind a Mesh if it not IsInitialized.");
    }
    // This calls glEnableVertexAttribArray and glVertexAttribPointer on the relevant things.
    m_vertex_buffer.Enable(attribute_locations);
    m_index_buffer.Bind();
  }
  void Draw () const {
    if (!IsInitialized()) {
      throw MeshException("Can't Draw a Mesh if it not IsInitialized.");
    }
    GL_THROW_UPON_ERROR(glDrawElements(m_draw_mode, m_index_count, GL_UNSIGNED_INT, 0));
  }
  // Unbinds this mesh.  Must pass in the same attribute_locations as to the call to Bind.
  // TODO: write about what GL operations actually happen.
  void Unbind (typename VertexBuffer::UniformLocations &attribute_locations) const {
    if (!IsInitialized()) {
      throw MeshException("Can't Unbind a Mesh if it not IsInitialized.");
    }
    m_index_buffer.Unbind();
    // This calls glDisableVertexAttribArray on the relevant things.
    m_vertex_buffer.Disable(attribute_locations);
  }

  // To be used for any draw mode -- the user is responsible for adding vertices using the convention
  // defined by the draw mode, and in particular is the only method allowable for adding vertices for
  // some draw modes.
  template <typename... Types_>
  void PushVertex (Types_... args) {
    if (IsInitialized()) {
      throw MeshException("Can't push vertex data into a Mesh that IsInitialized.");
    }
    m_intermediate_vertices.emplace_back(args...);
  }
  // To be used only when the draw mode is GL_LINES.  TODO: Figure out the appropriate constness/referenceness to use.
  void PushLine (const VertexAttributes &v0,
                 const VertexAttributes &v1) {
    if (IsInitialized()) {
      throw MeshException("Can't push vertex data into a Mesh that IsInitialized.");
    }
    if (m_draw_mode != GL_LINES) {
      throw MeshException("Mesh::PushLine is only defined if the draw mode is GL_LINES.");
    }
    m_intermediate_vertices.emplace_back(v0);
    m_intermediate_vertices.emplace_back(v1);
  }
  // To be used only when the draw mode is GL_TRIANGLES.
  void PushTriangle (const VertexAttributes &v0,
                     const VertexAttributes &v1,
                     const VertexAttributes &v2) {
    if (IsInitialized()) {
      throw MeshException("Can't push vertex data into a Mesh that IsInitialized.");
    }
    if (m_draw_mode != GL_TRIANGLES) {
      throw MeshException("Mesh::PushTriangle is only defined if the draw mode is GL_TRIANGLES.");
    }
    m_intermediate_vertices.emplace_back(v0);
    m_intermediate_vertices.emplace_back(v1);
    m_intermediate_vertices.emplace_back(v2);
  }
  // To be used only when the draw mode is GL_TRIANGLES.
  void PushQuad (const VertexAttributes &v0,
                 const VertexAttributes &v1,
                 const VertexAttributes &v2,
                 const VertexAttributes &v3) {
    if (IsInitialized()) {
      throw MeshException("Can't push vertex data into a Mesh that IsInitialized.");
    }
    if (m_draw_mode != GL_TRIANGLES) {
      throw MeshException("Mesh::PushQuad is only defined if the draw mode is GL_TRIANGLES.");
    }
    // Triangle 1
    m_intermediate_vertices.emplace_back(v0);
    m_intermediate_vertices.emplace_back(v1);
    m_intermediate_vertices.emplace_back(v2);
    // Triangle 2
    m_intermediate_vertices.emplace_back(v0);
    m_intermediate_vertices.emplace_back(v2);
    m_intermediate_vertices.emplace_back(v3);
  }
  // To be used only when the draw mode is GL_LINES_ADJACENCY.
  void PushLineAdjacency (const VertexAttributes &v0,
                          const VertexAttributes &v1,
                          const VertexAttributes &v2,
                          const VertexAttributes &v3) {
    if (IsInitialized()) {
      throw MeshException("Can't push vertex data into a Mesh that IsInitialized.");
    }
    if (m_draw_mode != GL_LINES_ADJACENCY) {
      throw MeshException("Mesh::PushLineAdjacency is only defined if the draw mode is GL_LINES_ADJACENCY.");
    }
    m_intermediate_vertices.emplace_back(v0);
    m_intermediate_vertices.emplace_back(v1);
    m_intermediate_vertices.emplace_back(v2);
    m_intermediate_vertices.emplace_back(v3);
  }
  // To be used only when the draw mode is GL_TRIANGLES_ADJACENCY.
  void PushTriangleAdjacency (const VertexAttributes &v0,
                              const VertexAttributes &v1,
                              const VertexAttributes &v2,
                              const VertexAttributes &v3,
                              const VertexAttributes &v4,
                              const VertexAttributes &v5) {
    if (IsInitialized()) {
      throw MeshException("Can't push vertex data into a Mesh that IsInitialized.");
    }
    if (m_draw_mode != GL_LINES) {
      throw MeshException("Mesh::PushTriangleAdjacency is only defined if the draw mode is GL_TRIANGLES_ADJACENCY.");
    }
    m_intermediate_vertices.emplace_back(v0);
    m_intermediate_vertices.emplace_back(v1);
    m_intermediate_vertices.emplace_back(v2);
    m_intermediate_vertices.emplace_back(v3);
    m_intermediate_vertices.emplace_back(v4);
    m_intermediate_vertices.emplace_back(v5);
  }
  // Gives direct access to the std::vector of intermediate vertices -- the data that will be
  // unique'ified to construct an indexed vertex buffer object and uploaded to the GPU.
  std::vector<VertexAttributes> &IntermediateVertices () { return m_intermediate_vertices; }
  // TODO: allow changing the m_vertex_buffer contents and re-uploading?

private:

  // The draw mode that will be used in Draw().
  GLenum m_draw_mode;
  // This intermediate storage for vertex data as it is being generated, and may be cleared during upload.
  std::vector<VertexAttributes> m_intermediate_vertices;
  // This is the vertex buffer object for uploaded vertex attribute data.
  VertexBuffer m_vertex_buffer;
  // This is the number of indices used to pass to glDrawElements when drawing the VBO.
  size_t m_index_count;
  // This is the buffer containing the index elements.
  Buffer m_index_buffer;
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
