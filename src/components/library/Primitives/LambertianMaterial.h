#pragma once

#include "Color.h"
#include "EigenTypes.h"
#include "Leap/GL/GLHeaders.h"
#include "Leap/GL/ShaderFrontend.h"

enum LambertianMaterialProperty {
  LIGHT_POSITION,
  DIFFUSE_LIGHT_COLOR,
  AMBIENT_LIGHT_COLOR,
  AMBIENT_LIGHTING_PROPORTION,
  TEXTURE_MAPPING_ENABLED,
  TEXTURE_UNIT_INDEX
};

template <LambertianMaterialProperty NAME_, GLenum GL_TYPE_, typename CppType_>
using LambertianMaterialUniform = Leap::GL::Uniform<LambertianMaterialProperty,NAME_,GL_TYPE_,CppType_>;

typedef ShaderFrontend<LambertianMaterialProperty,
                       LambertianMaterialUniform<LIGHT_POSITION,GL_FLOAT_VEC3,EigenTypes::Vector3f>,
                       LambertianMaterialUniform<DIFFUSE_LIGHT_COLOR,GL_FLOAT_VEC4,Color>,
                       LambertianMaterialUniform<AMBIENT_LIGHT_COLOR,GL_FLOAT_VEC4,Color>,
                       LambertianMaterialUniform<AMBIENT_LIGHTING_PROPORTION,GL_FLOAT,float>,
                       LambertianMaterialUniform<TEXTURE_MAPPING_ENABLED,GL_BOOL,GLint>,
                       LambertianMaterialUniform<TEXTURE_UNIT_INDEX,GL_SAMPLER_2D,GLint>> LambertianMaterial;
