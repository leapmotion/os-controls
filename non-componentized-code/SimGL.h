#ifndef __SimGL_h__
#define __SimGL_h__

#include "DataTypes.h"
#include "DataStructures/RadialSolids.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <QtOpenGL/qgl.h>
#ifdef __MACH__
// Mac OS X does not use freeglut
#include <GLUT/glut.h>
#else
#ifdef _MSC_VER
//Windows
#include <GL/GL.h>
#include <GL/GLU.h>
#else
//linux
#include <GL/glut.h>
#include <GL/freeglut.h>
#endif
#endif

class SimGL {
public:
 
  static void DrawCylinder(const EigenTypes::Vector3& center, const EigenTypes::Vector3& direction, double radius, double length, bool capped = false, bool wireFrame = false);
  static void DrawCylinder(const EigenTypes::Vector3& end, const EigenTypes::Vector3& direction, double radius, bool capped = false, bool wireFrame = false);
  static void DrawSphere(double radius);
  static void DrawRadialSolid(const RadialSolid& radialSolid);
  static void DrawCapsule(const Capsule& capsule, bool drawEnd = true, const EigenTypes::Matrix4x4& stretch = EigenTypes::Matrix4x4::Identity());
  static void DrawBiCapsule(const BiCapsule& bicapsule, bool drawEnd=true);
  static void DrawRoundedRectangle(const RoundedRectangle& rectangle);
  static void DrawNoodle(const Noodle& noodle, int numArcSegments=40);
  static void DrawCapsoodle(const Capsoodle& capsoodle);
  static void DrawSolidBox(const EigenTypes::Vector3& center, const EigenTypes::Vector3& size);
  static void DrawDevice(float width, float length, float height, float radius);

private:

  template<class T, class Fn, class... Args>
  static bool Draw(const RadialSolid& radialSolid, Fn pfn, Args... args);

};

struct AutoQuadric {
  AutoQuadric();
  ~AutoQuadric();
  GLUquadric* quadric;
};

#endif
