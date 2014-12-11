#include <gtest/gtest.h>
#include "Leap/GL/Projection.h"

using namespace EigenTypes;
using namespace Leap::GL;

class ProjectionTest : public testing::Test { };

namespace {

void ExpectVectorEq (const Vector3 &expected, const Vector3 &actual) {
  EXPECT_DOUBLE_EQ(expected(0), actual(0));
  EXPECT_DOUBLE_EQ(expected(1), actual(1));
  EXPECT_DOUBLE_EQ(expected(2), actual(2));
}

Vector3 TransformHomogeneous (const Matrix4x4 &projection, const Vector3 &v) {
  Vector4 p(projection * v.colwise().homogeneous());
  // ASSERT_DOUBLE_GT(std::numeric_limits<double>::epsilon(), p(3));
  return Vector3(p.block<3,1>(0,0) / p(3));
}

} // end of anonymous namespace

TEST_F(ProjectionTest, SymmetricViewBox_CornerCorrespondence) {
  // Verify that each corner of the oriented orthographic view box
  //   [-w/2,w/2]x[-h/2,h/2]x[-n,-f],
  // where w := width, h := height, n := near_clip_depth, and f := far_clip_depth,
  // maps to the corresponding corner of
  //   [-1,1]x[-1,1]x[-1,1].

  double w = 8.0;
  double h = 6.0;
  double n = 0.5;
  double f = 128.0;
  Matrix4x4 projection;
  Projection::SetOrthographic_UsingSymmetricViewBox(projection, w, h, n, f);

  ExpectVectorEq(Vector3(-1, -1, -1), TransformHomogeneous(projection, Vector3(-w/2, -h/2, -n)));
  ExpectVectorEq(Vector3(-1,  1, -1), TransformHomogeneous(projection, Vector3(-w/2,  h/2, -n)));
  ExpectVectorEq(Vector3( 1, -1, -1), TransformHomogeneous(projection, Vector3( w/2, -h/2, -n)));
  ExpectVectorEq(Vector3( 1,  1, -1), TransformHomogeneous(projection, Vector3( w/2,  h/2, -n)));
  ExpectVectorEq(Vector3(-1, -1,  1), TransformHomogeneous(projection, Vector3(-w/2, -h/2, -f)));
  ExpectVectorEq(Vector3(-1,  1,  1), TransformHomogeneous(projection, Vector3(-w/2,  h/2, -f)));
  ExpectVectorEq(Vector3( 1, -1,  1), TransformHomogeneous(projection, Vector3( w/2, -h/2, -f)));
  ExpectVectorEq(Vector3( 1,  1,  1), TransformHomogeneous(projection, Vector3( w/2,  h/2, -f)));
}

TEST_F(ProjectionTest, ViewBox_CornerCorrespondence) {
  // Verify that each corner of the oriented orthographic view box
  //   [l,r]x[b,t]x[-n,-f],
  // where w := width, h := height, n := near_clip_depth, and f := far_clip_depth,
  // maps to the corresponding corner of
  //   [-1,1]x[-1,1]x[-1,1].

  double l = 8.0;
  double r = 24.0;
  double b = -2.0;
  double t = 6.0;
  double n = 0.5;
  double f = 128.0;
  Matrix4x4 projection;
  Projection::SetOrthographic(projection, l, r, b, t, n, f);

  ExpectVectorEq(Vector3(-1, -1, -1), TransformHomogeneous(projection, Vector3(l, b, -n)));
  ExpectVectorEq(Vector3(-1,  1, -1), TransformHomogeneous(projection, Vector3(l, t, -n)));
  ExpectVectorEq(Vector3( 1, -1, -1), TransformHomogeneous(projection, Vector3(r, b, -n)));
  ExpectVectorEq(Vector3( 1,  1, -1), TransformHomogeneous(projection, Vector3(r, t, -n)));
  ExpectVectorEq(Vector3(-1, -1,  1), TransformHomogeneous(projection, Vector3(l, b, -f)));
  ExpectVectorEq(Vector3(-1,  1,  1), TransformHomogeneous(projection, Vector3(l, t, -f)));
  ExpectVectorEq(Vector3( 1, -1,  1), TransformHomogeneous(projection, Vector3(r, b, -f)));
  ExpectVectorEq(Vector3( 1,  1,  1), TransformHomogeneous(projection, Vector3(r, t, -f)));
}

TEST_F(ProjectionTest, CornerCorrespondence) {
  // Verify that each corner of the perspective view frustum with near-clip facet
  // width w, height h, and near and far clip depth n and f (which actually have z
  // coordinates -n and -f respectively), maps to the corresponding corner of
  // [-1,1]x[-1,1]x[-1,1].

  double w = 8.0;
  double h = 6.0;
  double n = 0.5;
  double f = 128.0;
  Matrix4x4 projection;
  Projection::SetPerspective_UsingSymmetricFrustumNearClipSize(projection, w, h, n, f);

  ExpectVectorEq(Vector3(-1, -1, -1), TransformHomogeneous(projection, Vector3(-w/2, -h/2, -n)));
  ExpectVectorEq(Vector3(-1,  1, -1), TransformHomogeneous(projection, Vector3(-w/2,  h/2, -n)));
  ExpectVectorEq(Vector3( 1, -1, -1), TransformHomogeneous(projection, Vector3( w/2, -h/2, -n)));
  ExpectVectorEq(Vector3( 1,  1, -1), TransformHomogeneous(projection, Vector3( w/2,  h/2, -n)));
  ExpectVectorEq(Vector3(-1, -1,  1), TransformHomogeneous(projection, Vector3(f/n * -w/2, f/n * -h/2, -f)));
  ExpectVectorEq(Vector3(-1,  1,  1), TransformHomogeneous(projection, Vector3(f/n * -w/2, f/n *  h/2, -f)));
  ExpectVectorEq(Vector3( 1, -1,  1), TransformHomogeneous(projection, Vector3(f/n *  w/2, f/n * -h/2, -f)));
  ExpectVectorEq(Vector3( 1,  1,  1), TransformHomogeneous(projection, Vector3(f/n *  w/2, f/n *  h/2, -f)));
}
