#include "SVGPrimitive.h"

#include <Eigen/StdVector>
#define NANOSVG_ALL_COLOR_KEYWORDS  // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>
#include <poly2tri.h>

struct Bezier {
  Eigen::Vector2f b[4];
};

class Curve {
  public:
    Curve(float tolerance = 1.0f);
    ~Curve();

    void Append(const Bezier& bezier);

    const std::vector<p2t::Point*>& Points() const { return m_points; }

  private:
    void Subdivide(const Bezier& bezier, Bezier& left, Bezier& right);
    bool IsSufficientlyFlat(const Bezier& bezier);
    float m_tolerance;

    std::vector<p2t::Point*> m_points;
};

Curve::Curve(float tolerance) : m_tolerance(16.0f*tolerance*tolerance) // 16*tolerance^2
{
}

Curve::~Curve()
{
}

void Curve::Append(const Bezier& bezier) {
  if (IsSufficientlyFlat(bezier)) {
    if (m_points.empty()) {
      m_points.push_back(new p2t::Point(bezier.b[0].x(), bezier.b[0].y()));
    }
    if (bezier.b[3].x() == m_points[0]->x &&
        bezier.b[3].y() == m_points[0]->y) {
      return;
    }
    m_points.push_back(new p2t::Point(bezier.b[3].x(), bezier.b[3].y()));
  } else {
    Bezier left, right;
    Subdivide(bezier, left, right);
    Append(left);
    Append(right);
  }
}

void Curve::Subdivide(const Bezier& bezier, Bezier& left, Bezier& right) {
  const Eigen::Vector2f middle = 0.5f*(bezier.b[1] + bezier.b[2]);
  left.b[0] = bezier.b[0];
  left.b[1] = 0.5f*(bezier.b[0] + bezier.b[1]);
  left.b[2] = 0.5f*(left.b[1] + middle);
  right.b[3] = bezier.b[3];
  right.b[2] = 0.5f*(bezier.b[2] + bezier.b[3]);
  right.b[1] = 0.5f*(middle + right.b[2]);
  left.b[3] = right.b[0] = 0.5f*(left.b[2] + right.b[1]);
}

bool Curve::IsSufficientlyFlat(const Bezier& bezier) {
  Eigen::Vector2f u = 3.0f*bezier.b[1] - 2.0f*bezier.b[0] - bezier.b[3];
  Eigen::Vector2f v = 3.0f*bezier.b[2] - 2.0f*bezier.b[3] - bezier.b[0];
  return (u.cwiseProduct(u).cwiseMax(v.cwiseProduct(v)).sum() < m_tolerance);
}

SVGPrimitive::SVGPrimitive(const std::string& svg) {
  std::string copy{svg};
  NSVGimage *image = nsvgParse(const_cast<char*>(copy.c_str()), "px", 96.0f);
  if (image) {
    for (NSVGshape* shape = image->shapes; shape != NULL; shape = shape->next) {
      std::vector<p2t::Point*> polylines;
      p2t::CDT* cdt = nullptr;

      SetAmbientFactor(1.0f);
      const uint32_t abgr = shape->fill.color;
      SetDiffuseColor(Color(static_cast<uint8_t>(abgr      ),
                            static_cast<uint8_t>(abgr >>  8),
                            static_cast<uint8_t>(abgr >> 16),
                            static_cast<uint8_t>(abgr >> 24)));

      for (NSVGpath* path = shape->paths; path != NULL; path = path->next) {
        Curve curve(0.5f);
        for (int i = 0; i < path->npts-1; i += 3) {
          const float* p = &path->pts[i*2];
          Bezier bezier;
          bezier.b[0] << p[0], p[1];
          bezier.b[1] << p[2], p[3];
          bezier.b[2] << p[4], p[5];
          bezier.b[3] << p[6], p[7];
          curve.Append(bezier);
        }
        const auto& polyline = curve.Points();
        if (!polyline.empty()) {
          polylines.reserve(polylines.size() + polyline.size());
          polylines.insert(polylines.end(), polyline.begin(), polyline.end());
          if (cdt) {
            cdt->AddHole(polyline); // We are assuming that any additional paths are holes -- FIXME
          } else {
            cdt = new p2t::CDT(polyline);
          }
        }
      }
      if (cdt) {
        cdt->Triangulate();
        auto triangles = cdt->GetTriangles();
        m_Geometry.CleanUpBuffers();
        stdvectorV3f& vertices = m_Geometry.Vertices();
        stdvectorV3f& normals = m_Geometry.Normals();
        for (const auto& triangle : triangles) {
          for (int i = 0; i < 3; i++) {
            p2t::Point& pt = *triangle->GetPoint(i);
            Vector3f point(pt.x, pt.y, 0.0f);
            vertices.push_back(point);
            normals.push_back(Vector3f::UnitZ());
          }
        }
        delete cdt;
        m_Geometry.UploadDataToBuffers();
      }
      for (auto point : polylines) {
        delete point;
      }
    }
    nsvgDelete(image);
  }
}

void SVGPrimitive::Draw(RenderState& renderState, TransformStack& transform_stack) const {
  ModelView& modelView = renderState.GetModelView();

  modelView.Push();
  const Transform &t = transform_stack.top();
  modelView.Translate(t.translation());
  modelView.Multiply(Matrix3x3(t.linear()));

  renderState.UploadMatrices();
  renderState.UploadMaterial(DiffuseColor(), AmbientFactor());

  m_Geometry.Draw(renderState, GL_TRIANGLES);

  modelView.Pop();
}
