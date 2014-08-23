#include "GLTestFramework.h"
#include "GLTexture2.h"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <vector>

class GLTexture2Test : public GLTestFramework_Headless { };

// Because GTest's ASSERT_NO_THROW doesn't print a message if available.
#define ASSERT_NO_THROW_(x) \
  try { \
    x; \
  } catch (const std::exception &e) { \
    std::cerr << "exception caught: " << e.what() << '\n'; \
    ASSERT_TRUE(false) << e.what(); \
  } catch (...) { \
    std::cerr << "exception caught (no message)\n"; \
    ASSERT_TRUE(false); \
  }

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

TEST_F(GLTexture2Test, NonEmptyTexture_std_vector) {
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
