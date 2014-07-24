#include "UtilGL.h"
#include "Utility/MathUtil.h"
#include "Visualization/GLIncludes.h"
#include "GLBuffer.h"

Projection::Projection() : m_matrix(Matrix4x4::Identity()) { }

const Matrix4x4& Projection::Matrix() const {
  return m_matrix;
}

#if !USE_GL_ES
void Projection::LoadFromCurrent() {
  glGetDoublev(GL_PROJECTION_MATRIX, m_matrix.data());
}

void Projection::SetCurrent() {
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(m_matrix.data());
}
#endif

void Projection::Perspective(double left, double bottom, double right, double top, double nearClip, double farClip) {
  const double denom = 1/(nearClip - farClip);
  m_matrix << 2/(right - left), 0, (right + left)/(right - left), 0,
    0, 2/(top - bottom), (top + bottom)/(top - bottom), 0,
    0, 0, (farClip + nearClip)*denom, 2*farClip*nearClip*denom,
    0, 0, -1, 0;
}

void Projection::Perspective(double hFovRadians, double widthOverHeight, double nearClip, double farClip) {
  const double wd2 = nearClip * std::tan(hFovRadians / 2.0);
  const double left = -wd2;
  const double right = wd2;
  const double top = wd2/widthOverHeight;
  const double bottom = -wd2/widthOverHeight;
  Perspective(left, bottom, right, top, nearClip, farClip);
}

void Projection::Orthographic(double left, double bottom, double right, double top) {
  m_matrix << 2/(right - left), 0, 0, (right + left)/(left - right),
    0, 2/(top - bottom), 0, (top + bottom)/(bottom - top),
    0, 0, 1, 0,
    0, 0, 0, 1;
}

Vector2 Projection::Project(const Vector3& point) const {
  Vector2 result = (m_matrix * Vector4(point.x(), point.y(), point.z(), 1.0)).head<2>();
  result.x() = (result.x() + 1)/2;
  result.y() = (result.y() + 1)/2;
  return result;
}

void Projection::SetUniform(int address) const {
#if USE_GL_ES
  glUniformMatrix4fv(address, 1, GL_FALSE, m_matrix.data());
#else
  glUniformMatrix4fv(address, 1, GL_FALSE, m_matrix.cast<float>().eval().data());
#endif
}

ModelView::ModelView() {
  m_stack.push_back(Matrix4x4::Identity());
}

const Matrix4x4& ModelView::Matrix() const {
  return m_stack.back();
}

#if !USE_GL_ES
void ModelView::LoadFromCurrent() {
  glGetDoublev(GL_MODELVIEW_MATRIX, m_stack.back().data());
}

void ModelView::SetCurrent() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixd(m_stack.back().data());
}
#endif

void ModelView::Reset() {
  m_stack.back().setIdentity();
}

void ModelView::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
  Matrix4x4& mat = m_stack.back();
  Vector3 z = (eye - center).normalized();
  Vector3 y = up;
  Vector3 x = y.cross(z).normalized();
  y = z.cross(x).normalized();
  mat.row(0) << x, center.x();
  mat.row(1) << y, center.y();
  mat.row(2) << z, center.z();
  mat.row(3) << 0.0, 0.0, 0.0, 1.0;
}

void ModelView::Translate(const Vector3& translation) {
  m_stack.back() *= MathUtil::TranslationMatrix(translation);
}

void ModelView::Rotate(const Vector3& axis, double angleRadians) {
  m_stack.back() *= MathUtil::RotationMatrix(axis, angleRadians);
}

void ModelView::Scale(const Vector3& scale) {
  m_stack.back() *= MathUtil::ScaleMatrix(scale);
}

void ModelView::Multiply(const Matrix4x4& transform) {
  m_stack.back() *= transform;
}

void ModelView::SetUniform(int address) const {
#if USE_GL_ES
  glUniformMatrix4fv(address, 1, GL_FALSE, m_stack.back().data());
#else
  glUniformMatrix4fv(address, 1, GL_FALSE, m_stack.back().cast<float>().eval().data());
#endif
}

void ModelView::Push() {
  m_stack.push_back(m_stack.back());
}

void ModelView::Pop() {
  assert(m_stack.size() > 1);
  m_stack.pop_back();
}

Vector4 UtilGL::GetViewport() {
  Vector4 viewport;
  glGetDoublev(GL_VIEWPORT, viewport.data());
  return viewport;
}

void UtilGL::ScreenToRay(const Vector2& point, const Vector4& viewport, const Projection& proj, const ModelView& modelView, Vector3* origin, Vector3* direction) {
  const Matrix4x4 projModelViewInv = (proj.Matrix() * modelView.Matrix()).inverse();
  Vector4 in, out;
  in[0] = 2.0 * ((point.x() - viewport[0])/viewport[2]) - 1.0;
  in[1] = 2.0 * ((point.y() - viewport[1])/viewport[3]) - 1.0;
  in[2] = -1.0;
  in[3] = 1.0;
  out = projModelViewInv * in;
  Vector3 nearPoint = out.head<3>() / out[3];
  in[2] = 1.0;
  out = projModelViewInv * in;
  Vector3 farPoint = out.head<3>() / out[3];
  *origin = nearPoint;
  *direction = (farPoint - nearPoint).normalized();
}

void UtilGL::ModelViewProjectionToUniform(const Projection& proj, const ModelView& modelView, int address) {
  Matrix4x4 result = proj.Matrix() * modelView.Matrix();
#if USE_GL_ES
  glUniformMatrix4fv(address, 1, GL_FALSE, result.data());
#else
  glUniformMatrix4fv(address, 1, GL_FALSE, result.cast<float>().eval().data());
#endif
}

void UtilGL::NormalMatrixToUniform(const Projection& proj, const ModelView& modelView, int address) {
  Matrix4x4 result = (modelView.Matrix()).inverse().transpose();
#if USE_GL_ES
  glUniformMatrix4fv(address, 1, GL_FALSE, result.data());
#else
  glUniformMatrix4fv(address, 1, GL_FALSE, result.cast<float>().eval().data());
#endif
}

void UtilGL::Vector3ToUniform(const Vector3& vec, int address) {
#if USE_GL_ES
  glUniform3fv(address, 1, vec.data());
#else
  glUniform3fv(address, 1, vec.cast<float>().eval().data());
#endif
}

void UtilGL::Vector4ToUniform(const Vector4& vec, int address) {
#if USE_GL_ES
  glUniform4fv(address, 1, vec.data());
#else
  glUniform4fv(address, 1, vec.cast<float>().eval().data());
#endif
}

void UtilGL::DrawUnitSphere(int posAddr, int normalAddr) {
  static bool init = false;
  static GLBuffer vertexBuffer(GL_ARRAY_BUFFER);
  static GLBuffer normalBuffer(GL_ARRAY_BUFFER);
  static GLBuffer indexBuffer(GL_ELEMENT_ARRAY_BUFFER);
  static int numTriangles;
  if (!init) {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned short> indices;
    createSphere(20, vertices, normals, indices);

    vertexBuffer.create();
    vertexBuffer.bind();
    vertexBuffer.allocate((void*)(vertices.data()), (int)vertices.size()*sizeof(float), GL_STATIC_DRAW);
    vertexBuffer.release();

    normalBuffer.create();
    normalBuffer.bind();
    normalBuffer.allocate((void*)(normals.data()), (int)normals.size()*sizeof(float), GL_STATIC_DRAW);
    normalBuffer.release();

    indexBuffer.create();
    indexBuffer.bind();
    indexBuffer.allocate((void*)(indices.data()), (int)indices.size()*sizeof(unsigned short), GL_STATIC_DRAW);
    indexBuffer.release();

    numTriangles = (int)indices.size()/3;

    init = true;
  }
  vertexBuffer.bind();
  glEnableVertexAttribArray(posAddr);
  glVertexAttribPointer(posAddr, 3, GL_FLOAT, GL_TRUE, 0, 0);

  normalBuffer.bind();
  glEnableVertexAttribArray(normalAddr);
  glVertexAttribPointer(normalAddr, 3, GL_FLOAT, GL_TRUE, 0, 0);

  indexBuffer.bind();
  glDrawElements(GL_TRIANGLES, numTriangles*3, GL_UNSIGNED_SHORT, 0);
  indexBuffer.release();

  glDisableVertexAttribArray(posAddr);
  glDisableVertexAttribArray(normalAddr);

  normalBuffer.release();
  vertexBuffer.release();
}

void UtilGL::DrawUnitCylinder(int posAddr, int normalAddr) {
  static bool init = false;
  static GLBuffer vertexBuffer(GL_ARRAY_BUFFER);
  static GLBuffer normalBuffer(GL_ARRAY_BUFFER);
  static GLBuffer indexBuffer(GL_ELEMENT_ARRAY_BUFFER);
  static int numTriangles;
  if (!init) {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned short> indices;
    createCylinder(20, vertices, normals, indices);

    vertexBuffer.create();
    vertexBuffer.bind();
    vertexBuffer.allocate((void*)(vertices.data()), (int)vertices.size()*sizeof(float), GL_STATIC_DRAW);
    vertexBuffer.release();

    normalBuffer.create();
    normalBuffer.bind();
    normalBuffer.allocate((void*)(normals.data()), (int)normals.size()*sizeof(float), GL_STATIC_DRAW);
    normalBuffer.release();

    indexBuffer.create();
    indexBuffer.bind();
    indexBuffer.allocate((void*)(indices.data()), (int)indices.size()*sizeof(unsigned short), GL_STATIC_DRAW);
    indexBuffer.release();

    numTriangles = (int)indices.size()/3;

    init = true;
  }
  vertexBuffer.bind();
  glEnableVertexAttribArray(posAddr);
  glVertexAttribPointer(posAddr, 3, GL_FLOAT, GL_TRUE, 0, 0);

  normalBuffer.bind();
  glEnableVertexAttribArray(normalAddr);
  glVertexAttribPointer(normalAddr, 3, GL_FLOAT, GL_TRUE, 0, 0);

  indexBuffer.bind();
  glDrawElements(GL_TRIANGLES, numTriangles*3, GL_UNSIGNED_SHORT, 0);
  indexBuffer.release();

  glDisableVertexAttribArray(posAddr);
  glDisableVertexAttribArray(normalAddr);

  normalBuffer.release();
  vertexBuffer.release();
}

void UtilGL::DrawGridPlane(int posAddr, int colorAddr) {
  static bool init = false;
  static GLBuffer vertexBuffer(GL_ARRAY_BUFFER);
  static GLBuffer colorBuffer(GL_ARRAY_BUFFER);
  static GLBuffer indexBuffer(GL_ELEMENT_ARRAY_BUFFER);
  static int numLines;
  if (!init) {
    std::vector<float> vertices;
    std::vector<float> colors;
    std::vector<unsigned short> indices;
    createGridPlane(vertices, colors, indices);

    vertexBuffer.create();
    vertexBuffer.bind();
    vertexBuffer.allocate((void*)(vertices.data()), (int)vertices.size()*sizeof(float), GL_STATIC_DRAW);
    vertexBuffer.release();

    colorBuffer.create();
    colorBuffer.bind();
    colorBuffer.allocate((void*)(colors.data()), (int)colors.size()*sizeof(float), GL_STATIC_DRAW);
    colorBuffer.release();

    indexBuffer.create();
    indexBuffer.bind();
    indexBuffer.allocate((void*)(indices.data()), (int)indices.size()*sizeof(unsigned short), GL_STATIC_DRAW);
    indexBuffer.release();

    numLines = (int)indices.size()/2;

    init = true;
  }
  vertexBuffer.bind();
  glEnableVertexAttribArray(posAddr);
  glVertexAttribPointer(posAddr, 3, GL_FLOAT, GL_TRUE, 0, 0);

  colorBuffer.bind();
  glEnableVertexAttribArray(colorAddr);
  glVertexAttribPointer(colorAddr, 3, GL_FLOAT, GL_TRUE, 0, 0);

  indexBuffer.bind();
  glDrawElements(GL_LINES, numLines*2, GL_UNSIGNED_SHORT, 0);
  indexBuffer.release();

  glDisableVertexAttribArray(posAddr);
  glDisableVertexAttribArray(colorAddr);

  colorBuffer.release();
  vertexBuffer.release();
}

void UtilGL::DrawUnitBox(int posAddr, int normalAddr) {
  static bool init = false;
  static GLBuffer vertexBuffer(GL_ARRAY_BUFFER);
  static GLBuffer normalBuffer(GL_ARRAY_BUFFER);
  static GLBuffer indexBuffer(GL_ELEMENT_ARRAY_BUFFER);
  static int numTriangles;
  if (!init) {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned short> indices;
    createBox(vertices, normals, indices);

    vertexBuffer.create();
    vertexBuffer.bind();
    vertexBuffer.allocate((void*)(vertices.data()), (int)vertices.size()*sizeof(float), GL_STATIC_DRAW);
    vertexBuffer.release();

    normalBuffer.create();
    normalBuffer.bind();
    normalBuffer.allocate((void*)(normals.data()), (int)normals.size()*sizeof(float), GL_STATIC_DRAW);
    normalBuffer.release();

    indexBuffer.create();
    indexBuffer.bind();
    indexBuffer.allocate((void*)(indices.data()), (int)indices.size()*sizeof(unsigned short), GL_STATIC_DRAW);
    indexBuffer.release();

    numTriangles = (int)indices.size()/3;

    init = true;
  }
  vertexBuffer.bind();
  glEnableVertexAttribArray(posAddr);
  glVertexAttribPointer(posAddr, 3, GL_FLOAT, GL_TRUE, 0, 0);

  normalBuffer.bind();
  glEnableVertexAttribArray(normalAddr);
  glVertexAttribPointer(normalAddr, 3, GL_FLOAT, GL_TRUE, 0, 0);

  indexBuffer.bind();
  glDrawElements(GL_TRIANGLES, numTriangles*3, GL_UNSIGNED_SHORT, 0);
  indexBuffer.release();

  glDisableVertexAttribArray(posAddr);
  glDisableVertexAttribArray(normalAddr);

  normalBuffer.release();
  vertexBuffer.release();
}

void UtilGL::toVbo(const stdvectorV3f& vertices, const stdvectorV3f& normals, std::vector<float>& outVertices, std::vector<float>& outNormals, std::vector<unsigned short>& outIndices) {
  VertexIndexMap vmap;

  for (size_t i=0; i<vertices.size(); i++) {
    MapVertex cur(vertices[i], normals[i]);

    const VertexIndexMap::iterator result = vmap.find(cur);
    if (result == vmap.end()) {
      unsigned short newIndex = static_cast<unsigned short>(outVertices.size()/3);
      outVertices.push_back(cur.p[0]);
      outVertices.push_back(cur.p[1]);
      outVertices.push_back(cur.p[2]);
      outNormals.push_back(cur.n[0]);
      outNormals.push_back(cur.n[1]);
      outNormals.push_back(cur.n[2]);
      outIndices.push_back(newIndex);
      vmap[cur] = newIndex;
    } else {
      outIndices.push_back(result->second);
    }
  }
}

void UtilGL::createSphere(int resolution, std::vector<float>& vertices, std::vector<float>& normals, std::vector<unsigned short>& indices) {
  const float resFloat = static_cast<float>(resolution);
  const float pi = static_cast<float>(M_PI);
  const float twoPi = static_cast<float>(2.0 * M_PI);

  stdvectorV3f v;
  stdvectorV3f n;

  for (int w=0; w<resolution; w++) {
    for (int h=-resolution/2; h<resolution/2; h++) {
      const float inc1 = (w/resFloat) * twoPi;
      const float inc2 = ((w+1)/resFloat) * twoPi;
      const float inc3 = (h/resFloat) * pi;
      const float inc4 = ((h+1)/resFloat) * pi;

      const float x1 = std::sin(inc1);
      const float y1 = std::cos(inc1);
      const float z1 = std::sin(inc3);

      const float x2 = std::sin(inc2);
      const float y2 = std::cos(inc2);
      const float z2 = std::sin(inc4);

      const float r1 = std::cos(inc3);
      const float r2 = std::cos(inc4);

      v.push_back(Vector3f(r1*x1, z1, r1*y1));
      v.push_back(Vector3f(r1*x2, z1, r1*y2));
      v.push_back(Vector3f(r2*x2, z2, r2*y2));
      v.push_back(Vector3f(r1*x1, z1, r1*y1));
      v.push_back(Vector3f(r2*x2, z2, r2*y2));
      v.push_back(Vector3f(r2*x1, z2, r2*y1));

      n.push_back(Vector3f(r1*x1, z1, r1*y1).normalized());
      n.push_back(Vector3f(r1*x2, z1, r1*y2).normalized());
      n.push_back(Vector3f(r2*x2, z2, r2*y2).normalized());
      n.push_back(Vector3f(r1*x1, z1, r1*y1).normalized());
      n.push_back(Vector3f(r2*x2, z2, r2*y2).normalized());
      n.push_back(Vector3f(r2*x1, z2, r2*y1).normalized());
    }
  }

  toVbo(v, n, vertices, normals, indices);
}

void UtilGL::createCylinder(int resolution, std::vector<float>& vertices, std::vector<float>& normals, std::vector<unsigned short>& indices) {
  const float resFloat = static_cast<float>(resolution);
  const float twoPi = static_cast<float>(2.0 * M_PI);

  stdvectorV3f v;
  stdvectorV3f n;

  for (int w=0; w<resolution; w++) {
    const float inc1 = (w/resFloat) * twoPi;
    const float inc2 = ((w+1)/resFloat) * twoPi;

    const float c1 = std::cos(inc1);
    const float c2 = std::cos(inc2);
    const float s1 = std::sin(inc1);
    const float s2 = std::sin(inc2);

    for (int h=0; h<resolution; h++) {
      const float h1 = (h/resFloat);
      const float h2 = ((h+1)/resFloat);

      const Vector3f v1(c1, h1, s1);
      const Vector3f v2(c1, h2, s1);
      const Vector3f v3(c2, h1, s2);
      const Vector3f v4(c2, h2, s2);

      const Vector3f n1(c1, 0, s1);
      const Vector3f n2(c2, 0, s2);

      v.push_back(v1);
      v.push_back(v2);
      v.push_back(v3);

      v.push_back(v2);
      v.push_back(v3);
      v.push_back(v4);

      n.push_back(n1);
      n.push_back(n1);
      n.push_back(n2);

      n.push_back(n1);
      n.push_back(n2);
      n.push_back(n2);
    }
  }

  toVbo(v, n, vertices, normals, indices);
}

void UtilGL::createGridPlane(std::vector<float>& vertices, std::vector<float>& colors, std::vector<unsigned short>& indices) {
  static const float GRID_COLOR_R_DARK = 0.075f;
  static const float GRID_COLOR_G_DARK = 0.175f;
  static const float GRID_COLOR_B_DARK = 0.45f;
  static const float GRID_COLOR_R_LIGHT = 0.1125f;
  static const float GRID_COLOR_G_LIGHT = 0.2625f;
  static const float GRID_COLOR_B_LIGHT = 0.675f;
  static const float GRID_HEIGHT = 300.0f;
  static const float GRID_WIDTH = 500.0f;
  static const float GRID_SPACING = 50.0f;
  static const float GRID_SUBDIVISIONS = 5.0f;

  stdvectorV3f v;
  stdvectorV3f c;

  Vector3f curColor;

  float x0 = -GRID_WIDTH/2;
  float y0 = -GRID_HEIGHT/2;
  float z0 = -GRID_HEIGHT/2;
  float xAmt = GRID_WIDTH;
  float yAmt = GRID_HEIGHT;
  float inc = GRID_SPACING / GRID_SUBDIVISIONS;

  curColor << GRID_COLOR_R_LIGHT, GRID_COLOR_G_LIGHT, GRID_COLOR_B_LIGHT;

  for (float x=0; x<=xAmt; x+=GRID_SPACING) {
    for (float y=0; y<=yAmt; y+=GRID_SPACING) {
      v.push_back(Vector3f(x0, y0+y, z0));
      v.push_back(Vector3f(x0+xAmt, y0+y, z0));
      v.push_back(Vector3f(x0+x, y0, z0));
      v.push_back(Vector3f(x0+x, y0+yAmt, z0));

      c.push_back(curColor);
      c.push_back(curColor);
      c.push_back(curColor);
      c.push_back(curColor);
    }
  }

  curColor << GRID_COLOR_R_DARK, GRID_COLOR_G_DARK, GRID_COLOR_B_DARK;
  for (float x=0; x<xAmt; x+=GRID_SPACING) {
    for (float y=0; y<yAmt; y+=GRID_SPACING) {
      for (float offset=inc; offset<GRID_SPACING; offset+=inc) {
        v.push_back(Vector3f(x0, y0+y+offset, z0));
        v.push_back(Vector3f(x0+xAmt, y0+y+offset, z0));
        v.push_back(Vector3f(x0+x+offset, y0, z0));
        v.push_back(Vector3f(x0+x+offset, y0+yAmt, z0));

        c.push_back(curColor);
        c.push_back(curColor);
        c.push_back(curColor);
        c.push_back(curColor);
      }
    }
  }

  toVbo(v, c, vertices, colors, indices);
}

void UtilGL::createBox(std::vector<float>& vertices, std::vector<float>& normals, std::vector<unsigned short>& indices) {
  vertices.push_back(-1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(1.0f);
  vertices.push_back(-1.0f);

  normals = vertices;
  for (size_t i=0; i<vertices.size()/3; i++) {
    const size_t idx1 = 3*i;
    const size_t idx2 = 3*i + 1;
    const size_t idx3 = 3*i + 2;
    Vector3f vert(vertices[idx1], vertices[idx2], vertices[idx3]);
    vert.normalize();
    normals[idx1] = vert.x();
    normals[idx2] = vert.y();
    normals[idx3] = vert.z();
  }

  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(2);
  indices.push_back(2);
  indices.push_back(3);
  indices.push_back(0);
  indices.push_back(3);
  indices.push_back(2);
  indices.push_back(6);
  indices.push_back(6);
  indices.push_back(7);
  indices.push_back(3);
  indices.push_back(7);
  indices.push_back(6);
  indices.push_back(5);
  indices.push_back(5);
  indices.push_back(4);
  indices.push_back(7);
  indices.push_back(4);
  indices.push_back(0);
  indices.push_back(3);
  indices.push_back(3);
  indices.push_back(7);
  indices.push_back(4);
  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(5);
  indices.push_back(5);
  indices.push_back(4);
  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(5);
  indices.push_back(6);
  indices.push_back(6);
  indices.push_back(2);
  indices.push_back(1);
}
