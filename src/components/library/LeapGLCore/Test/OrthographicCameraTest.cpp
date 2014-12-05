#include <gtest/gtest.h>
#include "Leap/GL/OrthographicCamera.h"

using namespace EigenTypes;

class OrthographicCameraTest : public testing::Test { };

namespace {

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

} // end of anonymous namespace

TEST_F(OrthographicCameraTest, SymmetricViewBox_CornerCorrespondence) {
  // Verify that each corner of the oriented orthographic view box
  //   [-w/2,w/2]x[-h/2,h/2]x[-n,-f],
  // where w := width, h := height, n := near_clip_depth, and f := far_clip_depth,
  // maps to the corresponding corner of
  //   [-1,1]x[-1,1]x[-1,1].

  Leap::GL::OrthographicCamera camera;
  double w = 8.0;
  double h = 6.0;
  double n = 0.5;
  double f = 128.0;
  camera.SetSymmetricViewBox(w, h, n, f);

  Matrix4x4 projection = camera.ProjectionMatrix();
  ExpectVectorEq(Vector3(-1, -1, -1), TransformHomogeneous(projection, Vector3(-w/2, -h/2, -n)));
  ExpectVectorEq(Vector3(-1,  1, -1), TransformHomogeneous(projection, Vector3(-w/2,  h/2, -n)));
  ExpectVectorEq(Vector3( 1, -1, -1), TransformHomogeneous(projection, Vector3( w/2, -h/2, -n)));
  ExpectVectorEq(Vector3( 1,  1, -1), TransformHomogeneous(projection, Vector3( w/2,  h/2, -n)));
  ExpectVectorEq(Vector3(-1, -1,  1), TransformHomogeneous(projection, Vector3(-w/2, -h/2, -f)));
  ExpectVectorEq(Vector3(-1,  1,  1), TransformHomogeneous(projection, Vector3(-w/2,  h/2, -f)));
  ExpectVectorEq(Vector3( 1, -1,  1), TransformHomogeneous(projection, Vector3( w/2, -h/2, -f)));
  ExpectVectorEq(Vector3( 1,  1,  1), TransformHomogeneous(projection, Vector3( w/2,  h/2, -f)));
}

TEST_F(OrthographicCameraTest, ViewBox_CornerCorrespondence) {
  // Verify that each corner of the oriented orthographic view box
  //   [l,r]x[b,t]x[-n,-f],
  // where w := width, h := height, n := near_clip_depth, and f := far_clip_depth,
  // maps to the corresponding corner of
  //   [-1,1]x[-1,1]x[-1,1].

  Leap::GL::OrthographicCamera camera;
  double l = 8.0;
  double r = 24.0;
  double b = -2.0;
  double t = 6.0;
  double n = 0.5;
  double f = 128.0;
  camera.SetViewBox(l, r, b, t, n, f);

  Matrix4x4 projection = camera.ProjectionMatrix();
  ExpectVectorEq(Vector3(-1, -1, -1), TransformHomogeneous(projection, Vector3(l, b, -n)));
  ExpectVectorEq(Vector3(-1,  1, -1), TransformHomogeneous(projection, Vector3(l, t, -n)));
  ExpectVectorEq(Vector3( 1, -1, -1), TransformHomogeneous(projection, Vector3(r, b, -n)));
  ExpectVectorEq(Vector3( 1,  1, -1), TransformHomogeneous(projection, Vector3(r, t, -n)));
  ExpectVectorEq(Vector3(-1, -1,  1), TransformHomogeneous(projection, Vector3(l, b, -f)));
  ExpectVectorEq(Vector3(-1,  1,  1), TransformHomogeneous(projection, Vector3(l, t, -f)));
  ExpectVectorEq(Vector3( 1, -1,  1), TransformHomogeneous(projection, Vector3(r, b, -f)));
  ExpectVectorEq(Vector3( 1,  1,  1), TransformHomogeneous(projection, Vector3(r, t, -f)));
}
