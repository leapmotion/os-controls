#include "GLTestFramework.h"
#include "GLTexture2.h"
#include "GLTexture2Loader.h"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include "Resource.h"
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
  
  std::shared_ptr<GLTexture2> texture;
  GLTexture2Params params(width, height);
  GLTexture2PixelDataReference pixel_data(GL_RED, GL_UNSIGNED_BYTE, raw_pixels);
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
  
  std::shared_ptr<GLTexture2> texture;
  GLTexture2Params params(width, height);
  ASSERT_NO_THROW_(texture = std::make_shared<GLTexture2>(params, pixel_data));
  EXPECT_EQ(100, texture->Params().Width());
  EXPECT_EQ(120, texture->Params().Height());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Visible tests
///////////////////////////////////////////////////////////////////////////////////////////////////

class GLTexture2VisibleTest : public GLTestFramework_Visible { };

TEST_F(GLTexture2VisibleTest, ProcedurallyGenerated) {
  std::shared_ptr<GLTexture2> texture;
  
  if (true) {
    // Generate a texture procedurally for use in TexturedFrame.
    GLsizei width = 4;
    GLsizei height = 4;
    GLTexture2Params params(width, height, GL_RGBA8);
    params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
//     struct RgbPixel { uint8_t r, g, b; };
    struct RgbPixel { GLfloat r, g, b; };
    
//     GLTexture2PixelDataStorage<RgbPixel> pixel_data(GL_RGB, GL_UNSIGNED_BYTE, width*height);
//     std::vector<RgbPixel> &pixels = pixel_data.RawPixels();
//     std::vector<RgbPixel> pixels(16);
    RgbPixel pixels[16];
    // Make a simple bilinear gradient in green and blue.
    for (GLsizei v = 0; v < height; ++v) {
      for (GLsizei u = 0; u < width; ++u) {
//         uint32_t r = 0;
//         uint32_t g = u*255/(width-1);
//         uint32_t b = v*255/(height-1);
//         RgbPixel &pixel = pixels[v*width+u];
//         pixel = RgbPixel{uint8_t(r), uint8_t(g), uint8_t(b)};
//         std::cout << "pixel(" << u << ',' << v << ") = (" << uint32_t(pixel.r) << ',' << uint32_t(pixel.g) << ',' << uint32_t(pixel.b) << ")\n";
        pixels[v*width+u] = RgbPixel{0.0f, u/GLfloat(width-1), v/GLfloat(height-1)};
      }
    }
//     GLTexture2PixelDataReference pixel_data(GL_RGB, GL_UNSIGNED_BYTE, pixels);
//     std::cout << "sizeof(pixels) = " << sizeof(pixels) << ", sizeof(RgbPixel) = " << sizeof(RgbPixel) << '\n';
//     std::cout << "&pixels[0] = " << &pixels[0]
//               << ", static_cast<const void *>(&pixels[0]) = " << static_cast<const void *>(&pixels[0])
//               << ", reinterpret_cast<const void *>(&pixels[0]) = " << reinterpret_cast<const void *>(&pixels[0]) << '\n';
    GLTexture2PixelDataReference pixel_data(GL_RGB, GL_FLOAT, static_cast<const void *>(&pixels[0]), sizeof(pixels));
      
    texture = std::make_shared<GLTexture2>(params, pixel_data);
  } else {
    texture = Resource<GLTexture2>("public_domain_astronomy_1.jpg");
  }
  
  // render dummy geometry just as a test
//   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glDisable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
    
  static const GLuint VERTEX_COUNT = 4;
  const GLfloat param = 0.7f;//0.1f * std::sin(3.0f*m_time) + 0.7f;
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
  texture->Bind();
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, vertex_array);
  glTexCoordPointer(2, GL_FLOAT, 0, TEXTURE_COORD_ARRAY);
  glDrawArrays(GL_TRIANGLE_FAN, 0, VERTEX_COUNT);
  texture->Unbind();
  
  m_GLController.EndRender();
  m_SDLController.EndRender();
  
  SDL_Delay(1000); // Delay for 1000 milliseconds.
}

