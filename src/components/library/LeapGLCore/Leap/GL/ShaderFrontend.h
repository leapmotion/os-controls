#pragma once

#include "Leap/GL/Common.h"
#include "Leap/GL/GLShader.h"
#include "Leap/GL/Internal/Map.h"
#include "Leap/GL/Internal/ShaderFrontend.h"

namespace Leap {
namespace GL {

template <typename UniformNameType_, typename... UniformMappings_>
class ShaderFrontend {
private:
  typedef Internal::Typle_t<UniformMappings_...> UniformMappingsTyple;
  typedef typename Internal::OnEach_f<UniformMappingsTyple,Internal::UniformNameOf_f>::T UniformNames;
  typedef typename Internal::OnEach_f<UniformMappingsTyple,Internal::GlTypeMappingOf_f>::T GlTypeMappings;
  typedef typename Internal::OnEach_f<UniformMappingsTyple,Internal::CppTypeMappingOf_f>::T CppTypeMappings;
  typedef typename Internal::OnEach_f<UniformMappingsTyple,Internal::CppTypeOf_f>::T CppTypes;
  typedef Internal::TypeMap_t<GlTypeMappings> GlTypeMap;
  typedef Internal::TypeMap_t<CppTypeMappings> CppTypeMap;
  // template <UniformNameType_ NAME_> using UniformName = Value_t<UniformNameType_,NAME_>;
public:

  template <UniformNameType_ NAME_> struct IndexOfUniform_f { static const size_t V = Internal::IndexIn_f<UniformNames,Internal::Value_t<UniformNameType_,NAME_>>::V; };
  template <UniformNameType_ NAME_> struct GlTypeOfUniform_f { static const GLenum V = Internal::Eval_f<GlTypeMap,Internal::Value_t<UniformNameType_,NAME_>>::T::V; };
  template <UniformNameType_ NAME_> struct CppTypeOfUniform_f { typedef typename Internal::Eval_f<CppTypeMap,Internal::Value_t<UniformNameType_,NAME_>>::T T; };
  typedef Internal::Tuple_t<typename Internal::UniformTyple_f<GLint,Internal::Length_f<UniformMappingsTyple>::V>::T> UniformLocations;
  typedef Internal::Tuple_t<CppTypes> UniformValues;

  // TODO: maybe make this have resource semantics (initialize/destroy)
  // TODO: checking for typed uniforms
  template <typename... Types_>
  ShaderFrontend (const GLShader &shader, const UniformLocations &uniform_locations, Types_... args)
    : m_shader(shader)
    , m_uniform_locations(uniform_locations)
    , m_uniform_values(args...)
  { }

  template <UniformNameType_ NAME_>
  void SetUniform (typename CppTypeOfUniform_f<NAME_>::T const &value) const {
    assert(GLShader::CurrentlyBoundProgramHandle() == m_shader.ProgramHandle() && "This shader must currently be bound in order to upload a uniform.");
    m_shader.SetUniform<GlTypeOfUniform_f<NAME_>::V>(m_uniform_locations.template el<IndexOfUniform_f<NAME_>::V>(), value);
  }
  template <UniformNameType_ NAME_, typename... Types_>
  void SetUniform (Types_... args) const {
    assert(GLShader::CurrentlyBoundProgramHandle() == m_shader.ProgramHandle() && "This shader must currently be bound in order to upload a uniform.");
    m_shader.SetUniform<GlTypeOfUniform_f<NAME_>::V>(m_uniform_locations.template el<IndexOfUniform_f<NAME_>::V>(), args...);
  }

private:

  const GLShader &m_shader;
  const UniformLocations m_uniform_locations;
  UniformValues m_uniform_values;
};


/*

enum class MaterialPropertyName { DIFFUSE_COLOR, LIGHT_POSITIONS, ... };
ShaderFrontend<MaterialPropertyName,
               Uniform<DIFFUSE_COLOR,GL_FLOAT_VEC4,Rgba<GLfloat>>,
               UniformArray<LIGHT_POSITIONS,GL_FLOAT_VEC3,10,Vector3<GLfloat>>,
               ...> material(uniform_locations);

material.Set<DIFFUSE_COLOR>(Rgba<GLfloat>(1,0,0,1));

Vector3 light_positions[10] = ...; // This could be a std::array<Vector3,10> or std::vector<Vector3> of size 10.
material.Set<LIGHT_POSITIONS>(light_positions);
material.Set<LIGHT_POSITIONS>(3, 10, array_of_7_Vector3s);

for (size_t i = 0; i < 10; ++i) {
  material.Set<LIGHT_POSITIONS>(i, Vector3(expression involving i));
}

*/




} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
