#ifndef __Utilities_h__
#define __Utilities_h__

template <class T>
static inline T SmootherStep(const T& x) {
  // x is blending parameter between 0 and 1
  return x*x*x*(x*(x*6 - 15) + 10);
}

inline ci::Vec3f ToVec3f(const Vector3& vec) {
  return ci::Vec3f(static_cast<float>(vec.x()), static_cast<float>(vec.y()), static_cast<float>(vec.z()));
}

template <class T>
struct ExponentialFilter {
  ExponentialFilter() : first(true), lastTimeSeconds(0), targetFramerate(100) { }
  void Update(const T& data, double timeSeconds, float smoothStrength) {
    if (first || timeSeconds < lastTimeSeconds) {
      value = data;
      first = false;
    } else {
      const float dtExponent = static_cast<float>((timeSeconds - lastTimeSeconds) * targetFramerate);
      smoothStrength = std::pow(smoothStrength, dtExponent);
      assert(smoothStrength >= 0.0f && smoothStrength <= 1.0f);
      value = smoothStrength*value + (1.0f-smoothStrength)*data;
    }
    lastTimeSeconds = timeSeconds;
  }
  T value;
  bool first;
  double lastTimeSeconds;
  float targetFramerate;
};

template <int N>
class CategoricalFilter {
public:
  CategoricalFilter() {
    for (int i=0; i<N; i++) {
      m_categories[i].value = 0.0f;
    }
  }
  void Update(int num, double timeSeconds, float smoothStrength) {
    // num must be between 0 and N-1, inclusive
    assert(num >= 0 && num <= N-1);
    for (int i=0; i<N; i++) {
      m_categories[i].Update(i==num ? 1.0f : 0.0f, timeSeconds, smoothStrength);
    }
  }
  int FilteredCategory() const {
    float max = -1;
    int maxCategory = -1;
    for (int i=0; i<N; i++) {
      if (m_categories[i].value > max) {
        max = m_categories[i].value;
        maxCategory = i;
      }
    }
    return maxCategory;
  }
private:
  ExponentialFilter<float> m_categories[N];
};

static Matrix4x4 RotationMatrix(const Vector3& axis, double angle) {
  Matrix4x4 mat;
  const double c = std::cos(angle);
  const double s = std::sin(angle);
  const double C = (1 - c);
  mat << axis[0] * axis[0] * C + c, axis[0] * axis[1] * C - axis[2] * s, axis[0] * axis[2] * C + axis[1] * s, 0,
    axis[1] * axis[0] * C + axis[2] * s, axis[1] * axis[1] * C + c, axis[1] * axis[2] * C - axis[0] * s, 0,
    axis[2] * axis[0] * C - axis[1] * s, axis[2] * axis[1] * C + axis[0] * s, axis[2] * axis[2] * C + c, 0,
    0, 0, 0, 1;
  return mat;
}

static Matrix4x4 TranslationMatrix(const Vector3& translation) {
  Matrix4x4 mat = Matrix4x4::Identity();
  mat(0, 3) = translation[0];
  mat(1, 3) = translation[1];
  mat(2, 3) = translation[2];
  return mat;
}

static Matrix4x4 ScaleMatrix(const Vector3& scale) {
  Matrix4x4 mat = Matrix4x4::Identity();
  mat(0, 0) = scale[0];
  mat(1, 1) = scale[1];
  mat(2, 2) = scale[2];
  return mat;
}

static const int TIME_STAMP_TICKS_PER_SEC = 1000000;
static const double TIME_STAMP_SECS_TO_TICKS  = static_cast<double>(TIME_STAMP_TICKS_PER_SEC);
static const double TIME_STAMP_TICKS_TO_SECS  = 1.0/TIME_STAMP_SECS_TO_TICKS;

static const float RADIANS_TO_DEGREES = static_cast<float>(180.0 / M_PI);
static const float DEGREES_TO_RADIANS = static_cast<float>(M_PI / 180.0);

#endif
