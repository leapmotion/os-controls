#include "ShapesLayer.h"

#include <cmath>

#include "GLShaderLoader.h"
#include "GLTexture2Loader.h"
#include "Leap/GL/GLBuffer.h"
#include "Leap/GL/GLShader.h"
#include "Leap/GL/GLTexture2.h"
#include "Leap/GL/PerspectiveCamera.h"
#include "Resource.h"
#include "TextFile.h"
#include "TextFileLoader.h"

ShapesLayer::ShapesLayer ()
  :
  m_Width(640),
  m_Height(480),
  m_Image1(Resource<GLTexture2>("rewind.png")),
  m_Image2(Resource<GLTexture2>("playpause.png")),
  m_Image3(Resource<GLTexture2>("fastforward.png")),
  m_time(0)
{
  m_shader = Resource<GLShader>("material");
  m_shader->CheckForTypedAttribute("position", GL_FLOAT_VEC3, VariableIs::OPTIONAL_BUT_WARN);
  m_shader->CheckForTypedAttribute("normal", GL_FLOAT_VEC3, VariableIs::OPTIONAL_BUT_WARN);
  m_shader->CheckForTypedAttribute("tex_coord", GL_FLOAT_VEC2, VariableIs::OPTIONAL_BUT_WARN);

  m_Sphere1.SetRadius(5);
  m_Sphere1.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Rgba<float>(0.4f, 0.7f, 1.0f, 1.0f);
  m_Sphere1.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Rgba<float>(0.2f, 0.2f, 0.2f, 1.0f);
  m_Sphere1.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.3f;
  m_Sphere1.LocalProperties().AlphaMask() = 1.0f;

  m_Sphere2.SetRadius(5);
  m_Sphere2.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Rgba<float>(1.0f, 0.7f, 0.4f, 1.0f);
  m_Sphere2.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Rgba<float>(0.2f, 0.2f, 0.2f, 1.0f);
  m_Sphere2.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.3f;
  m_Sphere2.LocalProperties().AlphaMask() = 1.0f;

  m_Rect.SetSize(EigenTypes::Vector2(20, 10));
  m_Rect.Translation() = EigenTypes::Vector3::Zero();
  m_Rect.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Rgba<float>(1.0f, 1.0f, 1.0f, 0.3f);
  m_Rect.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Rgba<float>(1.0f, 1.0f, 1.0f, 0.3f);
  m_Rect.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.9f;
  m_Rect.SetTexture(Resource<GLTexture2>("playpause.png"));
  m_Rect.LocalProperties().AlphaMask() = 1.0f;

  m_Image1.Translation() = EigenTypes::Vector3(-50.0, -20.0, 0.0);
  m_Image1.LinearTransformation() = m_Image1.LinearTransformation() / 100.0;
  m_Image1.LocalProperties().AlphaMask() = 1.0f;
  m_Image2.Translation() = EigenTypes::Vector3(  0.0,  30.0, 0.0);
  m_Image2.LinearTransformation() = m_Image2.LinearTransformation() / 100.0;
  m_Image2.LocalProperties().AlphaMask() = 1.0f;
  m_Image3.Translation() = EigenTypes::Vector3( 50.0, -20.0, 0.0);
  m_Image3.LinearTransformation() = m_Image3.LinearTransformation() / 100.0;
  m_Image3.LocalProperties().AlphaMask() = 1.0f;
  
  m_Cylinder.SetHeight(25);
  m_Cylinder.SetRadius(4);
  m_Cylinder.Translation() = 40*EigenTypes::Vector3::UnitX();
  m_Cylinder.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Rgba<float>(0.7f, 1.0f, 0.4f, 1.0f);
  m_Cylinder.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Rgba<float>(0.2f, 0.2f, 0.2f, 1.0f);
  m_Cylinder.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.3f;
  m_Cylinder.LocalProperties().AlphaMask() = 1.0f;

  m_Disk.SetRadius(6);
  m_Disk.Translation() = -40*EigenTypes::Vector3::UnitX();
  m_Disk.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Rgba<float>(1.0f, 1.0f, 0.3f, 1.0f);
  m_Disk.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Rgba<float>(1.0f, 1.0f, 0.3f, 1.0f);
  m_Disk.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.9f;
  m_Disk.LocalProperties().AlphaMask() = 1.0f;

  m_Box.SetSize(EigenTypes::Vector3(10, 5, 3));
  m_Box.Translation() = EigenTypes::Vector3(-30, 20, 0);
  m_Box.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Rgba<float>(0.3f, 1.0f, 1.0f, 1.0f);
  m_Box.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Rgba<float>(0.2f, 0.2f, 0.2f, 1.0f);
  m_Box.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.5f;
  m_Box.LocalProperties().AlphaMask() = 1.0f;

  m_PartialDisk.Translation() = EigenTypes::Vector3(30, -20, 0);
  m_PartialDisk.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Rgba<float>(1.0f, 0.3f, 1.0f, 1.0f);
  m_PartialDisk.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Rgba<float>(1.0f, 0.3f, 1.0f, 1.0f);
  m_PartialDisk.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 1.0f;
  m_PartialDisk.LocalProperties().AlphaMask() = 1.0f;

  Resource<TextFile> svg("tiger.svg");
  m_SVG.Set(svg->Contents());
  m_SVG.LinearTransformation() = EigenTypes::Vector3(0.03, -0.03, 0.03).asDiagonal() * m_SVG.LinearTransformation();
  m_SVG.LocalProperties().AlphaMask() = 1.0f;

  std::shared_ptr<GLTexture2> frame_texture;
  {
    // Generate a texture procedurally for use in TexturedFrame.
    GLsizei width = 4;
    GLsizei height = 4;
    GLTexture2Params params(width, height, GL_RGB8);
    params.SetTexParameteri(GL_GENERATE_MIPMAP, GL_TRUE);
    params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    params.SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    params.SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    struct RgbPixel { uint8_t r, g, b; };
    std::vector<RgbPixel> pixels(width*height);
    GLTexture2PixelData pixel_data(GL_RGB, GL_UNSIGNED_BYTE, pixels.data(), pixels.size()*sizeof(RgbPixel));
    // Make a simple bilinear gradient in green and blue.
    for (GLsizei v = 0; v < height; ++v) {
      for (GLsizei u = 0; u < width; ++u) {
        uint32_t r = 0;
        uint32_t g = u*255/(width-1);
        uint32_t b = v*255/(height-1);
        pixels[v*width+u] = RgbPixel{uint8_t(r), uint8_t(g), uint8_t(b)};
      }
    }
    frame_texture = std::make_shared<GLTexture2>(params, pixel_data);
  }

  {
    EigenTypes::Vector3 center(-20, -10, 0);
    EigenTypes::Vector3 vertical_offset(0, 10, 0);
    EigenTypes::Vector3 horizontal_offset(10, 0, 0);

    m_TexturedFrameM.Translation() = center;
    m_TexturedFrameE.Translation() = center + horizontal_offset;
    m_TexturedFrameN.Translation() = center + vertical_offset;
    m_TexturedFrameW.Translation() = center - horizontal_offset;
    m_TexturedFrameS.Translation() = center - vertical_offset;

    m_TexturedFrameM.SetTexture(frame_texture);
    m_TexturedFrameE.SetTexture(frame_texture);
    m_TexturedFrameN.SetTexture(frame_texture);
    m_TexturedFrameW.SetTexture(frame_texture);
    m_TexturedFrameS.SetTexture(frame_texture);
  }
  
  {
    m_DropShadowContrastBackground.Translation() = EigenTypes::Vector3(30, 20, 0);
    m_DropShadowContrastBackground.SetSize(EigenTypes::Vector2(25, 25));
    m_DropShadowContrastBackground.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Rgba<float>(0.75f, 0.75f, 0.75f, 1.0f);
    
    m_DropShadow.Translation() = EigenTypes::Vector3(30, 20, 0);
    m_DropShadow.SetBasisRectangleSize(EigenTypes::Vector2(10, 10));
  }

  // The FOV and other parameters for the camera are set in Render.
  m_Camera = std::make_shared<PerspectiveCamera>();
  m_Renderer.SetCamera(m_Camera);
}

ShapesLayer::~ShapesLayer () {

}

// TEMP -- come up with better way to implement this
EigenTypes::Matrix3x3 RotationMatrixFromEulerAngles (EigenTypes::MATH_TYPE pitch, EigenTypes::MATH_TYPE yaw, EigenTypes::MATH_TYPE roll) {
  EigenTypes::Matrix3x3 m;
  m =   Eigen::AngleAxis<EigenTypes::MATH_TYPE>(pitch, EigenTypes::Vector3::UnitX())
      * Eigen::AngleAxis<EigenTypes::MATH_TYPE>(  yaw, EigenTypes::Vector3::UnitY())
      * Eigen::AngleAxis<EigenTypes::MATH_TYPE>( roll, EigenTypes::Vector3::UnitZ());
  return m;
}

void ShapesLayer::Update (TimeDelta real_time_delta) {
  m_time += real_time_delta;

  m_Sphere1.Translation() = 15*EigenTypes::Vector3(std::cos(m_time), std::sin(m_time), 0);

  m_Sphere2.Translation() = 25*EigenTypes::Vector3(std::cos(-m_time), std::sin(-m_time), 0);

  m_Rect.LinearTransformation() = RotationMatrixFromEulerAngles(0.66*m_time, 0, -0.5*m_time);

  m_Cylinder.LinearTransformation() = RotationMatrixFromEulerAngles(0, m_time*0.33, m_time*0.25);

  m_Disk.LinearTransformation() = RotationMatrixFromEulerAngles(0, 0.5*m_time, 0);

  m_Box.LinearTransformation() = RotationMatrixFromEulerAngles(0.9*m_time, 0, 0.4*m_time);

  m_PartialDisk.SetStartAngle(M_PI * (1 + std::sin(0.5*m_time)));
  m_PartialDisk.SetEndAngle(2 * M_PI * (1 + std::sin(0.5*m_time)));
  m_PartialDisk.SetInnerRadius(4 + std::cos(m_time));
  m_PartialDisk.SetOuterRadius(8 + std::cos(0.66*m_time));

  EigenTypes::Vector2 basis_rectangle_size(3, 3);
  m_TexturedFrameM.SetBasisRectangleSize(basis_rectangle_size);
  m_TexturedFrameM.SetRectangleEdgeOffset(TexturedFrame::Rectangle::OUTER, TexturedFrame::RectangleEdge::RIGHT,  1.0+std::sin(0.666*m_time));
  m_TexturedFrameM.SetRectangleEdgeOffset(TexturedFrame::Rectangle::OUTER, TexturedFrame::RectangleEdge::TOP,    1.0+std::sin(0.75*m_time));
  m_TexturedFrameM.SetRectangleEdgeOffset(TexturedFrame::Rectangle::OUTER, TexturedFrame::RectangleEdge::LEFT,   1.0+std::sin(0.5*m_time));
  m_TexturedFrameM.SetRectangleEdgeOffset(TexturedFrame::Rectangle::OUTER, TexturedFrame::RectangleEdge::BOTTOM, 1.0+std::sin(0.45*m_time));
  m_TexturedFrameM.SetRectangleEdgeOffset(TexturedFrame::Rectangle::INNER, TexturedFrame::RectangleEdge::RIGHT,  1.0+std::sin(0.666*m_time));
  m_TexturedFrameM.SetRectangleEdgeOffset(TexturedFrame::Rectangle::INNER, TexturedFrame::RectangleEdge::TOP,    1.0+std::sin(0.75*m_time));
  m_TexturedFrameM.SetRectangleEdgeOffset(TexturedFrame::Rectangle::INNER, TexturedFrame::RectangleEdge::LEFT,   1.0+std::sin(0.5*m_time));
  m_TexturedFrameM.SetRectangleEdgeOffset(TexturedFrame::Rectangle::INNER, TexturedFrame::RectangleEdge::BOTTOM, 1.0+std::sin(0.45*m_time));
  m_TexturedFrameE.SetBasisRectangleSize(basis_rectangle_size + EigenTypes::Vector2(std::cos(0.75*m_time), std::sin(0.6*m_time)));
  m_TexturedFrameN.SetBasisRectangleSize(basis_rectangle_size + EigenTypes::Vector2(std::cos(0.75*m_time), std::sin(0.6*m_time)));
  m_TexturedFrameW.SetBasisRectangleSize(basis_rectangle_size);
  m_TexturedFrameS.SetBasisRectangleSize(basis_rectangle_size);

  // This intentionally goes below zero in order to demonstrate that it can cope with invalid values.
  m_DropShadow.SetShadowRadius(4.0 + 6.0*std::sin(0.5*m_time));
}

void ShapesLayer::Render(TimeDelta real_time_delta) const {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);

  // set renderer projection matrix
  const double fovRadians = (M_PI / 180.0) * 60;
  const double widthOverHeight = static_cast<double>(m_Width)/static_cast<double>(m_Height);
  const double nearClip = 1.0;
  const double farClip = 10000.0;
  m_Camera->SetUsingFOVAndAspectRatio(fovRadians, widthOverHeight, nearClip, farClip);

  // set renderer modelview matrix
  const EigenTypes::Vector3 eyePos = 100*EigenTypes::Vector3::UnitZ();
  const EigenTypes::Vector3 lookAtPoint = EigenTypes::Vector3::Zero();
  const EigenTypes::Vector3 upVector = EigenTypes::Vector3::UnitY();
  m_Renderer.GetModelView().Reset();
  m_Renderer.GetModelView().LookAt(eyePos, lookAtPoint, upVector);

  // set light position
  const EigenTypes::Vector3f desiredLightPos(0, 10, 10);
  const EigenTypes::Vector3f lightPos = desiredLightPos - eyePos.cast<float>();
  m_shader->Bind();
  m_shader->UploadUniform<GL_FLOAT_VEC3>("light_position", lightPos);
  m_shader->Unbind();

  // draw primitives
  PrimitiveBase::DrawSceneGraph(m_Sphere1, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_Sphere2, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_Rect, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_Image1, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_Image2, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_Image3, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_Cylinder, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_Disk, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_Box, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_PartialDisk, m_Renderer);

  glDisable(GL_DEPTH_TEST);
  PrimitiveBase::DrawSceneGraph(m_SVG, m_Renderer);
  glEnable(GL_DEPTH_TEST);

  PrimitiveBase::DrawSceneGraph(m_TexturedFrameM, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_TexturedFrameE, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_TexturedFrameN, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_TexturedFrameW, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_TexturedFrameS, m_Renderer);
  
  glDisable(GL_DEPTH_TEST);
  PrimitiveBase::DrawSceneGraph(m_DropShadowContrastBackground, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_DropShadow, m_Renderer);
  glEnable(GL_DEPTH_TEST);
}
