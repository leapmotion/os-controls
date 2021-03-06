#include "GLTestFramework.h"
#include "GLTexture2.h"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Headless tests
///////////////////////////////////////////////////////////////////////////////////////////////////

class GLTexture2HeadlessTest : public GLTestFramework_Headless { };

TEST_F(GLTexture2HeadlessTest, EmptyTexture) {
  GLsizei width = 100;
  GLsizei height = 120;
  
  std::shared_ptr<GLTexture2> texture;
  GLTexture2Params params(width, height);
  ASSERT_NO_THROW_(texture = std::make_shared<GLTexture2>(params));
  EXPECT_EQ(100, texture->Params().Width());
  EXPECT_EQ(120, texture->Params().Height());
}

TEST_F(GLTexture2HeadlessTest, NonEmptyTexture_RawPointer) {
  GLsizei width = 100;
  GLsizei height = 120;
  // Generate some raw pixel data
  uint8_t *raw_pixel_data = new uint8_t[width*height];
  size_t raw_pixel_data_byte_count = width*height*sizeof(uint8_t);
  for (GLsizei x = 0; x < width; ++x) {
    for (GLsizei y = 0; y < height; ++y) {
      raw_pixel_data[y*width + x] = static_cast<uint8_t>(x*y);
    }
  }
  
  // Create the texture from the pixel data.
  std::shared_ptr<GLTexture2> texture;
  GLTexture2Params params(width, height);
  GLTexture2PixelDataReference pixel_data(GL_RED, GL_UNSIGNED_BYTE, raw_pixel_data, raw_pixel_data_byte_count);
  ASSERT_NO_THROW_(texture = std::make_shared<GLTexture2>(params, pixel_data));
  EXPECT_EQ(100, texture->Params().Width());
  EXPECT_EQ(120, texture->Params().Height());
}

TEST_F(GLTexture2HeadlessTest, NonEmptyTexture_reference_to_std_vector) {
  GLsizei width = 100;
  GLsizei height = 120;
  // Generate some raw pixel data
  std::vector<uint8_t> raw_pixels(width*height);
  for (GLsizei x = 0; x < width; ++x) {
    for (GLsizei y = 0; y < height; ++y) {
      raw_pixels[y*width + x] = static_cast<uint8_t>(x*y);
    }
  }
  
  // Create the texture from the pixel data.
  std::shared_ptr<GLTexture2> texture;
  GLTexture2Params params(width, height);
  GLTexture2PixelDataReference pixel_data(GL_RED, GL_UNSIGNED_BYTE, raw_pixels.data(), raw_pixels.size()*sizeof(uint8_t));
  ASSERT_NO_THROW_(texture = std::make_shared<GLTexture2>(params, pixel_data));
  EXPECT_EQ(100, texture->Params().Width());
  EXPECT_EQ(120, texture->Params().Height());
}

TEST_F(GLTexture2HeadlessTest, NonEmptyTexture_stored_std_vector) {
  GLsizei width = 100;
  GLsizei height = 120;
  // Create the pixel data storage and write some data to it.
  GLTexture2PixelDataStorage<uint8_t> pixel_data(GL_RED, GL_UNSIGNED_BYTE, width*height);
  for (GLsizei x = 0; x < width; ++x) {
    for (GLsizei y = 0; y < height; ++y) {
      pixel_data.RawPixels()[y*width + x] = static_cast<uint8_t>(x*y);
    }
  }
  
  // Create the texture from the pixel data.
  std::shared_ptr<GLTexture2> texture;
  GLTexture2Params params(width, height);
  ASSERT_NO_THROW_(texture = std::make_shared<GLTexture2>(params, pixel_data));
  EXPECT_EQ(100, texture->Params().Width());
  EXPECT_EQ(120, texture->Params().Height());
}

TEST_F(GLTexture2HeadlessTest, ExtractTexture) {
  GLsizei width = 100;
  GLsizei height = 120;
  // Create the pixel data storage and write some data to it.
  GLTexture2PixelDataStorage<uint8_t> pixel_data(GL_RED, GL_UNSIGNED_BYTE, width*height);
  for (GLsizei x = 0; x < width; ++x) {
    for (GLsizei y = 0; y < height; ++y) {
      pixel_data.RawPixels()[y*width + x] = static_cast<uint8_t>(x*y);
    }
  }

  // Create the texture from the pixel data.
  std::shared_ptr<GLTexture2> texture;
  GLTexture2Params params(width, height);
  ASSERT_NO_THROW_(texture = std::make_shared<GLTexture2>(params, pixel_data));
  EXPECT_EQ(100, texture->Params().Width());
  EXPECT_EQ(120, texture->Params().Height());
  
  // Create another pixel data storage object to extract the texture data into.
  GLTexture2PixelDataStorage<uint8_t> extracted_pixel_data(GL_RED, GL_UNSIGNED_BYTE, width*height);
  texture->ExtractTexture(extracted_pixel_data);
  
  // Ensure that the pixel data is identical.
  EXPECT_EQ(pixel_data.RawPixels(), extracted_pixel_data.RawPixels());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Visible tests
///////////////////////////////////////////////////////////////////////////////////////////////////

class GLTexture2VisibleTest : public GLTestFramework_Visible { };

struct RgbPixel { uint8_t r, g, b; };

void GenerateLuminancePixels (GLTexture2PixelDataStorage<uint8_t> &pixel_data, GLsizei width, GLsizei height) {
  std::vector<uint8_t> &pixels = pixel_data.RawPixels();
  // Make a simple pattern.
  for (GLsizei v = 0; v < height; ++v) {
    for (GLsizei u = 0; u < width; ++u) {
      pixels[v*width+u] = uint8_t(u*v);
    }
  }
}

void GenerateRgbPixels (GLTexture2PixelDataStorage<RgbPixel> &pixel_data, GLsizei width, GLsizei height) {
  std::vector<RgbPixel> &pixels = pixel_data.RawPixels();
  // Make a simple bilinear gradient in green and blue.
  for (GLsizei v = 0; v < height; ++v) {
    for (GLsizei u = 0; u < width; ++u) {
      uint32_t r = (u+v)*255/(width-1 + height-1);
      uint32_t g = u*255/(width-1);
      uint32_t b = v*255/(height-1);
      RgbPixel &pixel = pixels[v*width+u];
      pixel = RgbPixel{uint8_t(r), uint8_t(g), uint8_t(b)};
    }
  }
}

void RenderTexturedRectangle (const GLTexture2 &texture) {
  glDisable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
    
  static const GLuint VERTEX_COUNT = 4;
  const GLfloat param = 0.7f;
  const GLfloat vertex_array[VERTEX_COUNT*2] = {
    -param, -param,
      param, -param,
      param,  param,
    -param,  param
  };
  static const GLfloat TEXTURE_COORD_ARRAY[VERTEX_COUNT*2] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
  };
  texture.Bind();
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, vertex_array);
  glTexCoordPointer(2, GL_FLOAT, 0, TEXTURE_COORD_ARRAY);
  glDrawArrays(GL_TRIANGLE_FAN, 0, VERTEX_COUNT);
  texture.Unbind();
}

TEST_F(GLTexture2VisibleTest, ProcedurallyGeneratedLuminance) {
  std::shared_ptr<GLTexture2> texture;
  
  // Generate a texture procedurally.
  {
    GLsizei width = 200;
    GLsizei height = 120;
    GLTexture2Params params(width, height, GL_LUMINANCE);
    params.SetTexParameteri(GL_GENERATE_MIPMAP, GL_TRUE);
    params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    params.SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    params.SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    GLTexture2PixelDataStorage<uint8_t> pixel_data(GL_LUMINANCE, GL_UNSIGNED_BYTE, width*height);
    GenerateLuminancePixels(pixel_data, width, height);
    texture = std::make_shared<GLTexture2>(params, pixel_data);
  }

  RenderTexturedRectangle(*texture);
  // Finish the frame before delaying. 
  m_GLController.EndRender();
  m_SDLController.EndRender();
  
  SDL_Delay(1000); // Delay so the human's pitiful visual system can keep up.
}

TEST_F(GLTexture2VisibleTest, ProcedurallyGeneratedRed) {
  std::shared_ptr<GLTexture2> texture;
  
  // Generate a texture procedurally.
  {
    GLsizei width = 200;
    GLsizei height = 120;
    GLTexture2Params params(width, height, GL_LUMINANCE);
    params.SetTexParameteri(GL_GENERATE_MIPMAP, GL_TRUE);
    params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    params.SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    params.SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    GLTexture2PixelDataStorage<uint8_t> pixel_data(GL_RED, GL_UNSIGNED_BYTE, width*height);
    GenerateLuminancePixels(pixel_data, width, height);
    texture = std::make_shared<GLTexture2>(params, pixel_data);
  }

  RenderTexturedRectangle(*texture);
  // Finish the frame before delaying. 
  m_GLController.EndRender();
  m_SDLController.EndRender();
  
  SDL_Delay(1000); // Delay so the human's pitiful visual system can keep up.
}

TEST_F(GLTexture2VisibleTest, ProcedurallyGeneratedRGB) {
  std::shared_ptr<GLTexture2> texture;
  
  // Generate a texture procedurally.
  {
    GLsizei width = 4;
    GLsizei height = 4;
    GLTexture2Params params(width, height, GL_RGBA8);
    params.SetTexParameteri(GL_GENERATE_MIPMAP, GL_TRUE);
    params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    params.SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    params.SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    GLTexture2PixelDataStorage<RgbPixel> pixel_data(GL_RGB, GL_UNSIGNED_BYTE, width*height);
    GenerateRgbPixels(pixel_data, width, height);
    texture = std::make_shared<GLTexture2>(params, pixel_data);
  }

  RenderTexturedRectangle(*texture);
  // Finish the frame before delaying. 
  m_GLController.EndRender();
  m_SDLController.EndRender();
  
  SDL_Delay(1000); // Delay so the human's pitiful visual system can keep up.
}

// This tests using glPixelStorei parameters (via GLTexture2PixelData::PixelStoreiParameterMap)
// to extract only a subregion out of the pixel data, rather than use the whole thing.
TEST_F(GLTexture2VisibleTest, ProcedurallyGenerated_WithStrideAndOffset) {
  std::shared_ptr<GLTexture2> texture;
  
  // Generate a texture procedurally.
  {
    GLsizei texture_width = 2;
    GLsizei texture_height = 2;
    GLTexture2Params params(texture_width, texture_height, GL_RGBA8);
    params.SetTexParameteri(GL_GENERATE_MIPMAP, GL_TRUE);
    params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    params.SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    params.SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    GLsizei pixel_data_width = 4;
    GLsizei pixel_data_height = 4;
    GLTexture2PixelDataStorage<RgbPixel> pixel_data(GL_RGB, GL_UNSIGNED_BYTE, pixel_data_width*pixel_data_height);
    GenerateRgbPixels(pixel_data, pixel_data_width, pixel_data_height);
    // This should specify the middle 2x2 subregion of pixels for the 2x2 texture.
    pixel_data.SetPixelStoreiParameter(GL_UNPACK_ROW_LENGTH, pixel_data_width);
    pixel_data.SetPixelStoreiParameter(GL_UNPACK_SKIP_PIXELS, 1);
    pixel_data.SetPixelStoreiParameter(GL_UNPACK_SKIP_ROWS, 1);
    texture = std::make_shared<GLTexture2>(params, pixel_data);
  }

  RenderTexturedRectangle(*texture);
  // Finish the frame before delaying. 
  m_GLController.EndRender();
  m_SDLController.EndRender();
  
  SDL_Delay(1000); // Delay so the human's pitiful visual system can keep up.
}

