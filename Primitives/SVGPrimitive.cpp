#include "SVGPrimitive.h"

#include <Eigen/StdVector>
#define NANOSVG_ALL_COLOR_KEYWORDS  // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>
#include <polypartition.h>

#include <cfloat>
#include <cassert>

struct Bezier {
  Eigen::Vector2f b[4];
};

class Curve {
  public:
    Curve(float tolerance = 1.0f);
    ~Curve();

    void Append(const Bezier& bezier);

    const std::vector<TPPLPoint>& Points() const { return m_points; }

  private:
    void Subdivide(const Bezier& bezier, Bezier& left, Bezier& right);
    bool IsSufficientlyFlat(const Bezier& bezier);
    float m_tolerance;

    std::vector<TPPLPoint> m_points;
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
      TPPLPoint point;
      point.x = bezier.b[0].x();
      point.y = bezier.b[0].y();
      m_points.emplace_back(std::move(point));
    }
    TPPLPoint point;
    point.x = bezier.b[3].x();
    point.y = bezier.b[3].y();
    const auto dx = point.x - m_points[0].x;
    if (std::abs(dx) < FLT_EPSILON) {
      const auto dy = point.y - m_points[0].y;
      if (std::abs(dy) < FLT_EPSILON) {
        return;
      }
    }
    m_points.emplace_back(std::move(point));
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

SVGPrimitive::SVGPrimitive(const std::string& svg) :
  m_Image(nullptr),
  m_RecomputeGeometry(false)
{
  if (!svg.empty()) {
    Set(svg);
  }
}

SVGPrimitive::~SVGPrimitive()
{
  if (m_Image) {
    nsvgDelete(m_Image);
  }
}

void SVGPrimitive::Draw(RenderState& renderState, TransformStack& transform_stack) const {
  if (m_RecomputeGeometry) {
    const_cast<SVGPrimitive*>(this)->RecomputeChildren(); // This objects children need to be recomputed
  }
}

void SVGPrimitive::Set(const std::string& svg)
{
  if (m_Image) {
    nsvgDelete(m_Image);
    m_Image = nullptr;
    Children().clear();
  }
  std::string svgCopy{svg}; // Make a copy so that nanosvg can modify its contents (horrors)
  m_Image = nsvgParse(const_cast<char*>(svgCopy.c_str()), "px", 96.0f);
  if (m_Image) {
    m_RecomputeGeometry = true;
  }
}

void SVGPrimitive::RecomputeChildren() {
  m_RecomputeGeometry = false;
  if (m_Image) {
    Children().clear();
    for (NSVGshape* shape = m_Image->shapes; shape != NULL; shape = shape->next) {
      if (shape->fill.type == NSVG_PAINT_COLOR) {
        // Fill
      } else if (shape->fill.type == NSVG_PAINT_NONE) {
        // Stoke -- FIXME
        continue;
      } else {
        // Gradient -- FIXME
        continue;
      }
      std::list<TPPLPoly> polys;

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
        const auto& points = curve.Points();
        if (!points.empty()) {
          TPPLPoly poly;
          size_t numPoints = points.size();

          poly.Init(static_cast<long>(numPoints));
          for (size_t i = 0; i < numPoints; i++) {
            poly[i] = points[i];
          }
          // We are assuming that ONLY the last path is not a hole
          if (path->next) {
            poly.SetHole(true);
            poly.SetOrientation(TPPL_CW);
          } else {
            poly.SetHole(false);
            poly.SetOrientation(TPPL_CCW);
          }
          polys.emplace_back(std::move(poly));
        }
      }
      if (!polys.empty()) {
        TPPLPartition partition;
        std::list<TPPLPoly> triangles;

        if (!partition.Triangulate_EC(&polys, &triangles)) {
          continue; // Failed to triangulate!
        }

        auto genericShape = std::shared_ptr<GenericShape>(new GenericShape());

        auto& geometry = genericShape->Geometry();
        const uint32_t abgr = shape->fill.color;

        geometry.CleanUpBuffers();

        genericShape->SetAmbientFactor(1.0f);
        genericShape->SetDiffuseColor(Color(static_cast<uint8_t>(abgr      ),
                                            static_cast<uint8_t>(abgr >>  8),
                                            static_cast<uint8_t>(abgr >> 16),
                                            static_cast<uint8_t>(abgr >> 24)));
        stdvectorV3f& vertices = geometry.Vertices();
        stdvectorV3f& normals = geometry.Normals();
        for (auto& triangle : triangles) {
          assert(triangle.GetNumPoints() == 3);
          for (int i = 0; i < 3; i++) {
            Vector3f point(static_cast<float>(triangle[i].x), static_cast<float>(triangle[i].y), 0.0f);
            vertices.push_back(point);
            normals.push_back(Vector3f::UnitZ());
          }
        }
        geometry.UploadDataToBuffers();

        AddChild(genericShape);
      }
    }
  }
}
