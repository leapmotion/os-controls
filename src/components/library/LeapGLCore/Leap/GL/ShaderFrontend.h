#pragma once

#include "Leap/GL/Common.h"
#include "Leap/GL/GLHeaders.h"
#include "Leap/GL/GLShader.h"
#include "Leap/GL/Internal/Map.h"
#include "Leap/GL/Internal/ShaderFrontend.h"
#include "Leap/GL/Internal/UniformTraits.h"
#include <sstream>

namespace Leap {
namespace GL {

template <typename UniformNameType_, typename... UniformMappings_>
class ShaderFrontend {
private:

  typedef Internal::Typle_t<UniformMappings_...> UniformMappingsTyple;
  static const size_t UNIFORM_COUNT = Internal::Length_f<UniformMappingsTyple>::V;
  typedef typename Internal::OnEach_f<UniformMappingsTyple,Internal::UniformNameOf_f>::T UniformNames;
  typedef typename Internal::OnEach_f<UniformMappingsTyple,Internal::GlTypeMappingOf_f>::T GlTypeMappings;
  typedef typename Internal::OnEach_f<UniformMappingsTyple,Internal::ArrayLengthMappingOf_f>::T ArrayLengthMappings;
  typedef typename Internal::OnEach_f<UniformMappingsTyple,Internal::CppTypeMappingOf_f>::T CppTypeMappings;
  typedef typename Internal::OnEach_f<UniformMappingsTyple,Internal::MatrixStorageConventionMappingOf_f>::T MatrixStorageConventionMappings;
  typedef Internal::TypeMap_t<GlTypeMappings> GlTypeMap;
  typedef Internal::TypeMap_t<ArrayLengthMappings> ArrayLengthMap;
  typedef Internal::TypeMap_t<CppTypeMappings> CppTypeMap;
  typedef Internal::TypeMap_t<MatrixStorageConventionMappings> MatrixStorageConventionMap;
  template <UniformNameType_ NAME_> using UniformName_t = Internal::Value_t<UniformNameType_,NAME_>;

  // template <UniformNameType_ NAME_> struct IndexOfUniform_f { static const size_t V = Internal::IndexIn_f<UniformNames,UniformName_t<NAME_>>::V; };

public:

  template <UniformNameType_ NAME_> struct GlTypeOfUniform_f { static const GLenum V = Internal::Eval_f<GlTypeMap,UniformName_t<NAME_>>::T::V; };
  template <UniformNameType_ NAME_> struct ArrayLengthOfUniform_f { static const size_t V = Internal::Eval_f<ArrayLengthMap,UniformName_t<NAME_>>::T::V; };
  template <UniformNameType_ NAME_> struct CppTypeOfUniform_f { typedef typename Internal::Eval_f<CppTypeMap,UniformName_t<NAME_>>::T T; };
  template <UniformNameType_ NAME_> struct MatrixStorageConventionOfUniform_f { typedef typename Internal::Eval_f<MatrixStorageConventionMap,UniformName_t<NAME_>>::T T; };
  typedef Internal::Tuple_t<typename Internal::UniformTyple_f<std::string,Internal::Length_f<UniformMappingsTyple>::V>::T> UniformIds; // TODO: this should be Map_t
  typedef Internal::Tuple_t<typename Internal::UniformTyple_f<GLint,Internal::Length_f<UniformMappingsTyple>::V>::T> UniformLocations; // TODO: this should be Map_t
  typedef Internal::Map_t<Internal::TypeMap_t<CppTypeMappings>> UniformMap;

  // TODO: maybe make this have resource semantics (initialize/destroy)
  template <typename... Types_>
  ShaderFrontend (const GLShader &shader, const UniformIds &uniform_ids, Types_... args)
    : m_shader(shader)
    , m_uniform_map(args...)
  {
    // Store the uniform locations from the shader using the uniform names.
    for (size_t i = 0; i < UNIFORM_COUNT; ++i) {
      m_uniform_locations.as_array()[i] = glGetUniformLocation(m_shader.ProgramHandle(), uniform_ids.as_array()[i].c_str());
    }
    // Compile-time checking of types.
    Internal::CheckUniformTypes<UniformMappingsTyple>::Check();
    // Run-time checking of types.
    CheckType<0>(uniform_ids);
  }

  template <UniformNameType_ NAME_> typename CppTypeOfUniform_f<NAME_>::T const &Uniform () const { return m_uniform_map.template val<UniformName_t<NAME_>>(); }
  template <UniformNameType_ NAME_> typename CppTypeOfUniform_f<NAME_>::T &Uniform () { return m_uniform_map.template val<UniformName_t<NAME_>>(); }
  const UniformMap &Uniforms () const { return m_uniform_map; }
  UniformMap &Uniforms () { return m_uniform_map; }

  void UploadUniforms () const {
    assert(GLShader::CurrentlyBoundProgramHandle() == m_shader.ProgramHandle() && "This shader must be bound in order to upload uniforms.");
    UploadUniform<0>();
  }

private:

  template <size_t INDEX_>
  typename std::enable_if<(INDEX_<UNIFORM_COUNT)>::type CheckType (const UniformIds &uniform_ids) const {
    typedef typename Internal::Element_f<UniformNames,INDEX_>::T UniformName;
    static const GLenum GL_TYPE_ = Internal::Eval_f<GlTypeMap,UniformName>::T::V;
    static const size_t ARRAY_LENGTH = Internal::Eval_f<ArrayLengthMap,UniformName>::T::V;
    if (m_uniform_locations.template el<INDEX_>() != -1) {
      const auto &uniform_id = uniform_ids.template el<INDEX_>();
      auto it = m_shader.UniformInfoMap().find(uniform_id);
      assert(it != m_shader.UniformInfoMap().end() && "This should never happen.");
      assert(GLShader::OPENGL_3_3_UNIFORM_TYPE_MAP.find(GL_TYPE_) != GLShader::OPENGL_3_3_UNIFORM_TYPE_MAP.end() && "Invalid uniform type.");
      const auto &info = it->second;
      if (GL_TYPE_ != info.Type()) {
        throw ShaderException("For uniform \"" + uniform_id + ", ShaderFrontend was looking for type " + GLShader::OPENGL_3_3_UNIFORM_TYPE_MAP.at(GL_TYPE_) +
                              " but the actual type was " + GLShader::OPENGL_3_3_UNIFORM_TYPE_MAP.at(info.Type()) + '.');
      }
      if (ARRAY_LENGTH != info.Size()) {
        std::ostringstream out;
        out << "For uniform \"" << uniform_id << ", ShaderFrontend was looking for array length " << ARRAY_LENGTH <<
               " but the actual array length was " << info.Size() << '.';
        throw ShaderException(out.str());
      }
    }
    // Iterate.
    CheckType<INDEX_+1>(uniform_ids);
  }
  template <size_t INDEX_>
  typename std::enable_if<(INDEX_>=UNIFORM_COUNT)>::type CheckType (const UniformIds &uniform_ids) const {
    // Done with the iteration.
  }

  template <size_t INDEX_>
  typename std::enable_if<(INDEX_<UNIFORM_COUNT)>::type UploadUniform () const {
    typedef typename Internal::Element_f<UniformNames,INDEX_>::T UniformName;
    static const GLenum GL_TYPE_ = Internal::Eval_f<GlTypeMap,UniformName>::T::V;
    static const size_t ARRAY_LENGTH = Internal::Eval_f<ArrayLengthMap,UniformName>::T::V;
    static const MatrixStorageConvention MATRIX_STORAGE_CONVENTION = Internal::Eval_f<MatrixStorageConventionMap,UniformName>::T::V;
    // Upload the uniform.
    Internal::UniformizedInterface_UploadArray<GL_TYPE_,ARRAY_LENGTH,MATRIX_STORAGE_CONVENTION>(m_uniform_locations.template el<INDEX_>(), m_uniform_map.template val<UniformName>());
    // Iterate.
    UploadUniform<INDEX_+1>();
  }
  template <size_t INDEX_>
  typename std::enable_if<(INDEX_>=UNIFORM_COUNT)>::type UploadUniform () const {
    // Done with iteration.
  }

  const GLShader &m_shader;
  UniformLocations m_uniform_locations;
  UniformMap m_uniform_map;
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
