#include "GLTestFramework.h"
#include "GLTexture2.h"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <vector>

class GLTexture2Test : public GLTestFramework_Headless { };

TEST_F(GLTexture2Test, EmptyTexture) {
  GLsizei width = 100;
  GLsizei height = 120;
  
  std::shared_ptr<GLTexture2> texture;
  GLTexture2Params params(width, height);
  ASSERT_NO_THROW_(texture = std::make_shared<GLTexture2>(params));
  EXPECT_EQ(100, texture->Params().Width());
  EXPECT_EQ(120, texture->Params().Height());
}

TEST_F(GLTexture2Test, NonEmptyTexture_RawPointer) {
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

TEST_F(GLTexture2Test, NonEmptyTexture_reference_to_std_vector) {
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

TEST_F(GLTexture2Test, NonEmptyTexture_stored_std_vector) {
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
