#include "SimGL.h"

#define NUM_CYLINDER_SIDES 20

AutoQuadric::AutoQuadric() {
  quadric = gluNewQuadric();
}

AutoQuadric::~AutoQuadric() {
  gluDeleteQuadric(quadric);
}

void SimGL::DrawSphere(double radius) {
  AutoQuadric q;
  gluQuadricDrawStyle(q.quadric, GLU_FILL);
  glEnable(GL_LIGHTING);
  gluSphere(q.quadric, radius, NUM_CYLINDER_SIDES, NUM_CYLINDER_SIDES);
  glDisable(GL_LIGHTING);
}

void SimGL::DrawCylinder(const Vector3& center, const Vector3& direction, double radius, double length, bool capped, bool wireFrame) {
  AutoQuadric q;
  if (wireFrame) {
    gluQuadricDrawStyle(q.quadric, GLU_SILHOUETTE);
  } else {
    glEnable(GL_LIGHTING);
    gluQuadricDrawStyle(q.quadric, GLU_FILL);
  }
  double angle;
  Vector3 axis;
  MathUtil::GetRotation(Vector3(0, 0, 1), direction, axis, angle);
  if (direction[2] < 0) {
    angle *= -1;
    angle += OCU_PI;
  }
  glPushMatrix();
  glTranslated(center[0], center[1], center[2]);
  glRotated(RADIANS_TO_DEGREES*angle, axis[0], axis[1], axis[2]);
  glTranslated(0, 0, -length/2.0);
  gluCylinder(q.quadric, radius, radius, length, NUM_CYLINDER_SIDES, 1);
  if (capped) {
    gluDisk(q.quadric, 0, radius, NUM_CYLINDER_SIDES, 1);
    glTranslated(0, 0, length);
    gluDisk(q.quadric, 0, radius, NUM_CYLINDER_SIDES, 1);
  }
  glPopMatrix();
  if (!wireFrame) {
    glDisable(GL_LIGHTING);
  }
}

void SimGL::DrawCylinder(const Vector3& end, const Vector3& direction, double radius, bool capped, bool wireFrame) {
  AutoQuadric q;
  if (wireFrame) {
    gluQuadricDrawStyle(q.quadric, GLU_SILHOUETTE);
  } else {
    glEnable(GL_LIGHTING);
    gluQuadricDrawStyle(q.quadric, GLU_FILL);
  }
  const double length = direction.norm();
  double angle;
  Vector3 axis;
  MathUtil::GetRotation(Vector3(0, 0, 1), direction/length, axis, angle);
  if (direction[2] < 0) {
    angle *= -1;
    angle += OCU_PI;
  }
  glPushMatrix();
  glTranslated(end[0], end[1], end[2]);
  glRotated(RADIANS_TO_DEGREES*angle, axis[0], axis[1], axis[2]);
  gluCylinder(q.quadric, radius, radius, length, NUM_CYLINDER_SIDES, 1);
  if (capped) {
    gluDisk(q.quadric, 0, radius, NUM_CYLINDER_SIDES, 1);
    glTranslated(0, 0, length);
    gluDisk(q.quadric, 0, radius, NUM_CYLINDER_SIDES, 1);
  }
  glPopMatrix();
  if (!wireFrame) {
    glDisable(GL_LIGHTING);
  }
}

template<class RP>
void DrawRadialPolygon(const RP& rp) {
  AutoQuadric q;
  const Vector3& center = rp.center();
  const Vector3& axis1 = rp.axis1();
  const Vector3& axis2 = rp.axis2();
  const double rpScale = axis1.norm();
  double radius = rp.radius() / rpScale;

  glPushMatrix();
  glTranslated(center.x(), center.y(), center.z());

  //Calculate matrix transform
  Vector3 normal = axis1.cross(axis2) / rpScale;
  Matrix4x4 mat(Matrix4x4::Identity());
  mat.block<3, 3>(0, 0) << axis1, axis2, normal;
  glMultMatrixd(mat.data());

  //Draw center box
  glEnable(GL_LIGHTING);
  glBegin(GL_TRIANGLE_FAN);
  glNormal3d(0, 0, 1);
  for (int i=0; i<RP::NumSides; i++) {
    const Vector2& v = rp.vertex(i);
    glVertex3d(v.x(), v.y(), radius);
  }
  glEnd();
  glBegin(GL_TRIANGLE_FAN);
  glNormal3d(0, 0, -1);
  for (int i=0; i<RP::NumSides; i++) {
    const Vector2& v = rp.vertex(i);
    glVertex3d(v.x(), v.y(), -radius);
  }
  glEnd();

  //Draw capsules
  const Vector2* p = &rp.vertex(RP::NumSides - 1);
  for (int i=0; i<RP::NumSides; i++) {
    const Vector2& v = rp.vertex(i);
    glPushMatrix();
    glTranslated(v.x(), v.y(), 0);
    const Vector3 diff(p->x()-v.x(), p->y()-v.y(), 0);
    SimGL::DrawCylinder(diff*0.5, diff.normalized(), radius, diff.norm());
    glEnable(GL_LIGHTING);
    gluSphere(q.quadric, radius, NUM_CYLINDER_SIDES, NUM_CYLINDER_SIDES);
    glPopMatrix();
    p = &v;
  }

  glPopMatrix();
  glDisable(GL_LIGHTING);
}

template<class T, class Fn, class... Args>
bool SimGL::Draw(const RadialSolid& radialSolid, Fn pfn, Args... args) {
  const T* pT = dynamic_cast<const T*>(&radialSolid);
  if (!pT)
    return false;

  pfn(*pT, args...);
  return true;
}

void SimGL::DrawRadialSolid(const RadialSolid& radialSolid) {
  // Do the first of the following sequence of operations:
  Draw<Capsule>(radialSolid, &SimGL::DrawCapsule, true, Matrix4x4::Identity()) ||
    Draw<RoundedRectangle>(radialSolid, &SimGL::DrawRoundedRectangle) ||
    Draw<Noodle>(radialSolid, &SimGL::DrawNoodle, 40) ||
    Draw<Capsoodle>(radialSolid, &SimGL::DrawCapsoodle) ||
    Draw<RadialPolygon<3>>(radialSolid, &DrawRadialPolygon<RadialPolygon<3>>) ||
    Draw<RadialPolygon<4>>(radialSolid, &DrawRadialPolygon<RadialPolygon<4>>) ||
    Draw<RadialPolygon<5>>(radialSolid, &DrawRadialPolygon<RadialPolygon<5>>) ||
    Draw<RadialPolygon<6>>(radialSolid, &DrawRadialPolygon<RadialPolygon<6>>) ||
    Draw<BiCapsule>(radialSolid, &SimGL::DrawBiCapsule, true);

  const RadialPoint* sphere = dynamic_cast<const RadialPoint*>(&radialSolid);
  if (sphere) {
    glPushMatrix();
    glTranslated(sphere->center().x(), sphere->center().y(), sphere->center().z());
    DrawSphere(sphere->radius());
    glPopMatrix();
  }
}

void SimGL::DrawCapsule(const Capsule& capsule, bool drawEnd, const Matrix4x4& stretch) {
  AutoQuadric q;
  const Vector3& direction = capsule.direction();
  const double radius = capsule.radius();
  const Vector3 center = capsule.center();
  const double length = capsule.length();
  gluQuadricDrawStyle(q.quadric, GLU_FILL);
  double angle;
  Vector3 axis;
  MathUtil::GetRotation(Vector3(0, 0, 1), direction, axis, angle);
  if (direction[2] < 0) {
    angle *= -1;
    angle += OCU_PI;
  }
  glPushMatrix();
  glTranslated(center[0], center[1], center[2]);
  glMultMatrixd(stretch.data());
  glRotated(RADIANS_TO_DEGREES*angle, axis[0], axis[1], axis[2]);
  glTranslated(0, 0, -length/2.0);
  glEnable(GL_LIGHTING);
  gluCylinder(q.quadric, radius, radius, length, NUM_CYLINDER_SIDES, 1);
  gluSphere(q.quadric, radius, NUM_CYLINDER_SIDES, NUM_CYLINDER_SIDES);
  if (drawEnd) {
    glTranslated(0, 0, length);
    gluSphere(q.quadric, radius, NUM_CYLINDER_SIDES, NUM_CYLINDER_SIDES);
  }
  glPopMatrix();
  glDisable(GL_LIGHTING);
}

void SimGL::DrawBiCapsule(const BiCapsule& bicapsule, bool drawEnd) {
  AutoQuadric q;
  const Vector3& direction = bicapsule.direction();
  const Vector3 center = bicapsule.center();
  const double length = bicapsule.length();
  const double r1 = bicapsule.radiusBig();
  const double r2 = bicapsule.radius();

  //Solve for the actual radii and offsets
  const double offset1 = r1*(r1-r2)/length;
  const double offset2 = r2*offset1/r1;
  const double radius1 = std::sqrt(r1*r1 - offset1*offset1);
  const double radius2 = r2*radius1/r1;

  gluQuadricDrawStyle(q.quadric, GLU_FILL);
  double angle;
  Vector3 axis;
  MathUtil::GetRotation(Vector3(0, 0, 1), direction, axis, angle);
  if (direction[2] < 0) {
    angle *= -1;
    angle += OCU_PI;
  }
  glPushMatrix();
  glTranslated(center[0], center[1], center[2]);
  glRotated(RADIANS_TO_DEGREES*angle, axis[0], axis[1], axis[2]);
  glPushMatrix();
  glTranslated(0, 0, -length/2.0);
  glEnable(GL_LIGHTING);
  gluSphere(q.quadric, r2, NUM_CYLINDER_SIDES, NUM_CYLINDER_SIDES);
  glTranslated(0, 0, -offset2);
  gluCylinder(q.quadric, radius2, radius1, length + offset2 - offset1, NUM_CYLINDER_SIDES, 1);
  glPopMatrix();
  if (drawEnd) {
    glTranslated(0, 0, length/2.0);
    gluSphere(q.quadric, r1, NUM_CYLINDER_SIDES, NUM_CYLINDER_SIDES);
  }
  glPopMatrix();
  glDisable(GL_LIGHTING);
}

void SimGL::DrawRoundedRectangle(const RoundedRectangle& rectangle) {
  AutoQuadric q;
  glEnable(GL_LIGHTING);

  const Vector3& center = rectangle.center();
  const Vector3& axis1 = rectangle.axis1();
  const Vector3& axis2 = rectangle.axis2();
  double radius = rectangle.radius();

  glPushMatrix();
  glTranslated(center.x(), center.y(), center.z());

  // calculate matrix transform
  Vector3 normal = axis1.cross(axis2).normalized();
  double aNorm = axis1.norm();
  double bNorm = axis2.norm();
  Matrix4x4 mat(Matrix4x4::Identity());
  mat.block<3, 3>(0, 0) << axis1/aNorm, axis2/bNorm, normal;
  glMultMatrixd(mat.data());

  // draw center box
  glBegin(GL_QUADS);
  glNormal3d(0, 0, 1);
  glVertex3d(-aNorm, -bNorm, radius);
  glVertex3d(aNorm, -bNorm, radius);
  glVertex3d(aNorm, bNorm, radius);
  glVertex3d(-aNorm, bNorm, radius);
  glNormal3d(0, 0, -1);
  glVertex3d(-aNorm, -bNorm, -radius);
  glVertex3d(aNorm, -bNorm, -radius);
  glVertex3d(aNorm, bNorm, -radius);
  glVertex3d(-aNorm, bNorm, -radius);
  glEnd();

  glPushMatrix();
  glTranslated(aNorm, -bNorm, 0);
  glRotated(-90, 1, 0, 0);
  gluCylinder(q.quadric, radius, radius, 2*bNorm, NUM_CYLINDER_SIDES, 1);
  gluSphere(q.quadric, radius, NUM_CYLINDER_SIDES, NUM_CYLINDER_SIDES);
  glPopMatrix();

  glPushMatrix();
  glTranslated(aNorm, bNorm, 0);
  glRotated(-90, 0, 1, 0);
  gluCylinder(q.quadric, radius, radius, 2*aNorm, NUM_CYLINDER_SIDES, 1);
  gluSphere(q.quadric, radius, NUM_CYLINDER_SIDES, NUM_CYLINDER_SIDES);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-aNorm, bNorm, 0);
  glRotated(90, 1, 0, 0);
  gluCylinder(q.quadric, radius, radius, 2*bNorm, NUM_CYLINDER_SIDES, 1);
  gluSphere(q.quadric, radius, NUM_CYLINDER_SIDES, NUM_CYLINDER_SIDES);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-aNorm, -bNorm, 0);
  glRotated(90, 0, 1, 0);
  gluCylinder(q.quadric, radius, radius, 2*aNorm, NUM_CYLINDER_SIDES, 1);
  gluSphere(q.quadric, radius, NUM_CYLINDER_SIDES, NUM_CYLINDER_SIDES);
  glPopMatrix();

  glPopMatrix();
  glDisable(GL_LIGHTING);
}

void SimGL::DrawNoodle(const Noodle& noodle, int numArcSegments) {
  AutoQuadric q;
  glEnable(GL_LIGHTING);
  const Vector3 normal = noodle.normal().normalized();
  const double arcAngle = noodle.arcAngle();
  const Vector3& center = noodle.center();
  const double segmentRadius = noodle.radius();
  const double arcRadius = noodle.arcRadius();
  const Vector3& sinAxis = noodle.sinAxis();

  // transform to proper position/orientation
  Matrix4x4 basis(Matrix4x4::Identity());
  basis.block<3, 3>(0, 0) << normal, -normal.cross(sinAxis), sinAxis;
  glPushMatrix();
  glTranslated(center.x(), center.y(), center.z());
  glMultMatrixd(basis.data());

  // precompute points for a circular ring (only needs to be done once)
  static const int NUM_RADIAL_SEGMENTS = 20;
  static Vector3 radialPoints[NUM_RADIAL_SEGMENTS];
  static bool radialComputed = false;
  if (!radialComputed) {
    double curRadialAngle = 0;
    double radialAngleInc = 2*M_PI/NUM_RADIAL_SEGMENTS;
    for (int i=0; i<NUM_RADIAL_SEGMENTS; i++) {
      radialPoints[i] << cos(curRadialAngle), sin(curRadialAngle), 0;
      curRadialAngle += radialAngleInc;
    }
    radialComputed = true;
  }

  // precompute rotation and translation based on the noodle parameters
  const double arcAngleInc = arcAngle/numArcSegments;
  const Matrix3x3 nextMat = MathUtil::RotationMatrix(Vector3::UnitX(), arcAngleInc).block<3, 3>(0, 0);
  const Vector3 arcVec(0, arcRadius, 0);

  // precompute the points on the next ring
  Vector3 nextPoints[NUM_RADIAL_SEGMENTS];
  Vector3 nextNormals[NUM_RADIAL_SEGMENTS];
  for (int i=0; i<NUM_RADIAL_SEGMENTS; i++) {
    nextPoints[i] = nextMat*(segmentRadius*radialPoints[i] + arcVec) - arcVec;
    nextNormals[i] = nextMat*radialPoints[i];
  }

  // draw start point sphere
  glPushMatrix();
  glTranslated(arcVec.x(), arcVec.y(), arcVec.z());
  gluSphere(q.quadric, segmentRadius, NUM_RADIAL_SEGMENTS, NUM_RADIAL_SEGMENTS);
  glPopMatrix();

  // draw partial torus
  Vector3 curPos, curNormal;
  double curArcAngle = 0;
  for (int i=0; i<numArcSegments; i++) {
    double nextAngle = curArcAngle + arcAngleInc;
    glPushMatrix();
    glRotated(RADIANS_TO_DEGREES*curArcAngle, 1, 0, 0);
    glTranslated(arcVec.x(), arcVec.y(), arcVec.z());

    // draw quads for each vertex of this ring connected to the corresponding vertex in the next ring
    glBegin(GL_QUAD_STRIP);
    for (int j=0; j<=NUM_RADIAL_SEGMENTS; j++) {
      int idx = j%NUM_RADIAL_SEGMENTS;
      glNormal3dv(radialPoints[idx].data());
      glVertex3dv((segmentRadius*radialPoints[idx]).eval().data());
      glNormal3dv(nextNormals[idx].data());
      glVertex3dv(nextPoints[idx].data());
    }
    glEnd();

    glPopMatrix();
    curArcAngle = nextAngle;
  }

  // draw end point sphere
  glPushMatrix();
  glRotated(RADIANS_TO_DEGREES*curArcAngle, 1, 0, 0);
  glTranslated(arcVec.x(), arcVec.y(), arcVec.z());
  gluSphere(q.quadric, segmentRadius, NUM_RADIAL_SEGMENTS, NUM_RADIAL_SEGMENTS);
  glPopMatrix();

  glPopMatrix();
  glDisable(GL_LIGHTING);
}

void SimGL::DrawCapsoodle(const Capsoodle& capsoodle) {
  Matrix4x4 stretch = Matrix4x4::Identity();
  for (int i=0; i<3; i++) {
    DrawCapsule(capsoodle.GetCapsule(i), i==2, stretch);
  }
}

void SimGL::DrawDevice(float width, float length, float height, float radius) {
  AutoQuadric q;
  static const int SIDES = 20;

  glPushMatrix();

  DrawSolidBox(Vector3::Zero(), Vector3(length-2*radius, height, width));
  DrawSolidBox(Vector3::Zero(), Vector3(length, height, width-2*radius));
  glRotated(90, 1, 0, 0);

  glPushMatrix();
  glTranslated(length/2-radius, 10, -height/2);
  gluCylinder(q.quadric, radius, radius, height, SIDES, 1);
  gluDisk(q.quadric, 0, radius, SIDES, 1);
  glTranslated(0, 0, height);
  gluDisk(q.quadric, 0, radius, SIDES, 1);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-(length/2-radius), 10, -height/2);
  gluCylinder(q.quadric, radius, radius, height, SIDES, 1);
  gluDisk(q.quadric, 0, radius, SIDES, 1);
  glTranslated(0, 0, height);
  gluDisk(q.quadric, 0, radius, SIDES, 1);
  glPopMatrix();

  glPushMatrix();
  glTranslated(length/2-radius, -10, -height/2);
  gluCylinder(q.quadric, radius, radius, height, SIDES, 1);
  gluDisk(q.quadric, 0, radius, SIDES, 1);
  glTranslated(0, 0, height);
  gluDisk(q.quadric, 0, radius, SIDES, 1);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-(length/2-radius), -10, -height/2);
  gluCylinder(q.quadric, radius, radius, height, SIDES, 1);
  gluDisk(q.quadric, 0, radius, SIDES, 1);
  glTranslated(0, 0, height);
  gluDisk(q.quadric, 0, radius, SIDES, 1);
  glPopMatrix();

  glPopMatrix();
}

void SimGL::DrawSolidBox(const Vector3& center, const Vector3& size) {
  glPushMatrix();
  glTranslated(center.x(), center.y(), center.z());
  glScaled(size.x(), size.y(), size.z());
  glTranslated(-0.5f, -0.5f, 0.5f);
  glBegin(GL_QUADS);
  // front
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glVertex3f(1.0f, 1.0f, 0.0f);
  glVertex3f(0.0f, 1.0f, 0.0f);
  // back
  glVertex3f(0.0f, 0.0f, -1.0f);
  glVertex3f(1.0f, 0.0f, -1.0f);
  glVertex3f(1.0f, 1.0f, -1.0f);
  glVertex3f(0.0f, 1.0f, -1.0f);
  // right
  glVertex3f(1.0f, 0.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, -1.0f);
  glVertex3f(1.0f, 1.0f, -1.0f);
  glVertex3f(1.0f, 1.0f, 0.0f);
  // left
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, -1.0f);
  glVertex3f(0.0f, 1.0f, -1.0f);
  glVertex3f(0.0f, 1.0f, 0.0f);
  // top
  glVertex3f(0.0f, 1.0f, 0.0f);
  glVertex3f(1.0f, 1.0f, 0.0f);
  glVertex3f(1.0f, 1.0f, -1.0f);
  glVertex3f(0.0f, 1.0f, -1.0f);
  // bottom
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, -1.0f);
  glVertex3f(0.0f, 0.0f, -1.0f);
  glEnd();
  glPopMatrix();
}
