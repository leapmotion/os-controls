#ifndef __UtilGL_h__
#define __UtilGL_h__

#include RVALUE_HEADER
#include <Eigen/Dense>
#include <Eigen/StdVector>
#include <vector>
#include <map>

#define USE_GL_ES 0

#if USE_GL_ES
typedef Eigen::Matrix<float, 2, 1> Vector2;
typedef Eigen::Matrix<float, 3, 1> Vector3;
typedef Eigen::Matrix<float, 4, 1> Vector4;
typedef Eigen::Matrix<float, 4, 4> Matrix4x4;
#else
typedef Eigen::Matrix<double, 2, 1> Vector2;
typedef Eigen::Matrix<double, 3, 1> Vector3;
typedef Eigen::Matrix<double, 4, 1> Vector4;
typedef Eigen::Matrix<double, 4, 4> Matrix4x4;
#endif
typedef Eigen::Matrix<float, 2, 1> Vector2f;
typedef Eigen::Matrix<float, 3, 1> Vector3f;
typedef Eigen::Matrix<float, 4, 1> Vector4f;
typedef Eigen::Matrix<float, 4, 4> Matrix4x4f;

class Projection {
public:
  Projection();
  const Matrix4x4& Matrix() const;
#if !USE_GL_ES
  void LoadFromCurrent();
  void SetCurrent();
#endif
  void Perspective(double left, double bottom, double right, double top, double nearClip, double farClip);
  void Perspective(double hFovRadians, double widthOverHeight, double nearClip, double farClip);
  void Orthographic(double left, double bottom, double right, double top);
  Vector2 Project(const Vector3& point) const;
  void SetUniform(int address) const;

EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  Matrix4x4 m_matrix;
};

class ModelView {
public:
  ModelView();
  const Matrix4x4& Matrix() const;
#if !USE_GL_ES
  void LoadFromCurrent();
  void SetCurrent();
#endif
  void Reset();
  void LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
  void Translate(const Vector3& translation);
  void Rotate(const Vector3& axis, double angleRadians);
  void Scale(const Vector3& scale);
  void Multiply(const Matrix4x4& transform);
  void SetUniform(int address) const;
  void Push();
  void Pop();
private:
  std::vector<Matrix4x4, Eigen::aligned_allocator<Matrix4x4> > m_stack;
};

class UtilGL {
public:
  static Vector4 GetViewport();
  static void ScreenToRay(const Vector2& point, const Vector4& viewport, const Projection& proj, const ModelView& modelView, Vector3* origin, Vector3* direction);
  static void ModelViewProjectionToUniform(const Projection& proj, const ModelView& modelView, int address);
  static void NormalMatrixToUniform(const Projection& proj, const ModelView& modelView, int address);
  static void Vector3ToUniform(const Vector3& vec, int address);
  static void Vector4ToUniform(const Vector4& vec, int address);
  static void DrawUnitSphere(int posAddr, int normalAddr);
  static void DrawUnitCylinder(int posAddr, int normalAddr);
  static void DrawGridPlane(int posAddr, int colorAddr);
  static void DrawUnitBox(int posAddr, int normalAddr);
private:

  typedef Eigen::Matrix<float, 3, 1> Vector3f;
  typedef Eigen::Matrix<float, 3, 3> Matrix3x3f;
  typedef std::vector<Vector3f, Eigen::aligned_allocator<Vector3f> > stdvectorV3f;

  struct MapVertex {
    MapVertex(const Vector3f& pos, const Vector3f& normal) {
      p[0] = pos[0];
      p[1] = pos[1];
      p[2] = pos[2];
      n[0] = normal[0];
      n[1] = normal[1];
      n[2] = normal[2];
    }
    float p[3];
    float n[3];
    bool operator<(const MapVertex& other) const {
      return memcmp((const void*)this, (const void*)(&other), sizeof(MapVertex)) > 0;
    }
  };

  typedef std::map<MapVertex, unsigned short, std::less<MapVertex>, Eigen::aligned_allocator<std::pair<MapVertex, unsigned short> > > VertexIndexMap;

  static void toVbo(const stdvectorV3f& vertices, const stdvectorV3f& normals, std::vector<float>& outVertices, std::vector<float>& outNormals, std::vector<unsigned short>& outIndices);
  static void createSphere(int resolution, std::vector<float>& vertices, std::vector<float>& normals, std::vector<unsigned short>& indices);
  static void createCylinder(int resolution, std::vector<float>& vertices, std::vector<float>& normals, std::vector<unsigned short>& indices);
  static void createGridPlane(std::vector<float>& vertices, std::vector<float>& colors, std::vector<unsigned short>& indices);
  static void createBox(std::vector<float>& vertices, std::vector<float>& normals, std::vector<unsigned short>& indices);

};

#endif
