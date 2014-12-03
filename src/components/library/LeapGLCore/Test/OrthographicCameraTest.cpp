#include <gtest/gtest.h>
#include "Leap/GL/OrthographicCamera.h"

using namespace EigenTypes;

class OrthographicCameraTest : public testing::Test { };

void ExpectVectorEq (const Vector3 &expected, const Vector3 &actual) {
  EXPECT_DOUBLE_EQ(expected(0), actual(0));
  EXPECT_DOUBLE_EQ(expected(1), actual(1));
  EXPECT_DOUBLE_EQ(expected(2), actual(2));
}
// #define EXPECT_VECTOR3_EQ(expected, actual) EXPECT_DOUBLE_EQ(0.0, ((expected) - (actual)).squaredNorm())

Vector3 TransformHomogeneous (const Matrix4x4 &projection, const Vector3 &v) {
  Vector4 p(projection * v.colwise().homogeneous());
  // ASSERT_DOUBLE_GT(std::numeric_limits<double>::epsilon(), p(3));
  return Vector3(p.block<3,1>(0,0) / p(3));
}

TEST_F(OrthographicCameraTest, CornerCorrespondence) {
  // Verify that each corner of the orthographic view box
  //   [-w/2, w/2]x[-h/2, h/2]x[n, f],
  // where w := width, h := height, n := near_clip_depth, and f := far_clip_depth,
  // maps to the corresponding corner of
  //   [-1,1]x[-1,1]x[-1,1].

  Leap::GL::OrthographicCamera camera;
  double w = 8.0;
  double h = 6.0;
  double n = 0.5;
  double f = 128.0;
  camera.SetViewBox(w, h, n, f);

  Matrix4x4 projection = camera.ProjectionMatrix();
  ExpectVectorEq(Vector3(-1, -1, -1), TransformHomogeneous(projection, Vector3(-w/2, -h/2, n)));
  ExpectVectorEq(Vector3(-1,  1, -1), TransformHomogeneous(projection, Vector3(-w/2,  h/2, n)));
  ExpectVectorEq(Vector3( 1, -1, -1), TransformHomogeneous(projection, Vector3( w/2, -h/2, n)));
  ExpectVectorEq(Vector3( 1,  1, -1), TransformHomogeneous(projection, Vector3( w/2,  h/2, n)));
  ExpectVectorEq(Vector3(-1, -1,  1), TransformHomogeneous(projection, Vector3(-w/2, -h/2, f)));
  ExpectVectorEq(Vector3(-1,  1,  1), TransformHomogeneous(projection, Vector3(-w/2,  h/2, f)));
  ExpectVectorEq(Vector3( 1, -1,  1), TransformHomogeneous(projection, Vector3( w/2, -h/2, f)));
  ExpectVectorEq(Vector3( 1,  1,  1), TransformHomogeneous(projection, Vector3( w/2,  h/2, f)));
}
