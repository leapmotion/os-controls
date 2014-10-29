#pragma once

#include "Color.h"
#include "EigenTypes.h"
#include "gl_glext_glu.h"
#include "ScopeGuard.h"

namespace Leap {
namespace GL {

class GLShader;

// The Material class is a container for particular uniform values which can then be
// uploaded to a shader all at once.  A "material" shader must have the following uniforms:
//   uniform vec3 light_position;
//   uniform vec4 diffuse_light_color;
//   uniform vec4 ambient_light_color;
//   uniform float ambient_lighting_proportion;
//   uniform bool use_texture;
//   uniform sampler2D texture;
// Material should only be concerned with fragment shading.  Uploading a Material's state
// is done via the method UploadUniforms.
class GLMaterial {
public:

  // This function checks the given shader for the required uniforms.  If any are missing,
  // an exception will be thrown.
  static void CheckShaderForUniforms (const GLShader &shader);

  // Constructs a Material with reasonable default values.
  GLMaterial ();

  // Query material properties.
  const EigenTypes::Vector3f& LightPosition () const { return m_light_position; } // TODO: move this elsewhere -- it doesn't belong here
  const Color& DiffuseLightColor () const { return m_diffuse_light_color; }
  const Color& AmbientLightColor () const { return m_ambient_light_color; }
  float AmbientLightingProportion () const { return m_ambient_lighting_proportion; }

  // Modifiers for the properties of a material.
  void SetLightPosition (const EigenTypes::Vector3f &p) { m_light_position = p; } // TODO: move this elsewhere -- it doesn't belong here
  void SetDiffuseLightColor (const Color &c) { m_diffuse_light_color = c; }
  void SetAmbientLightColor (const Color &c) { m_ambient_light_color = c; }
  void SetAmbientLightingProportion (float f) { m_ambient_lighting_proportion = f; }
  void SetUseTexture (bool b) { m_use_texture = b; }
  void SetTexture (GLint texture_unit_index) { m_texture_unit_index = texture_unit_index; }

  // Alpha mask is not technically part of the material state.
  void UploadUniforms (const GLShader &shader, float alpha_mask, BindFlags bind_flags) const;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:

  EigenTypes::Vector3f m_light_position;
  Color m_diffuse_light_color;
  Color m_ambient_light_color;
  float m_ambient_lighting_proportion;
  bool m_use_texture;
  GLint m_texture_unit_index;
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
