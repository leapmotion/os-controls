#pragma once

// This is a simple animation utility class that will perform Poisson smoothing.
// The class is templated and can be used with double, float, Vector3, or anything
// that overloads addition and scalar multiplication.
// When NUM_ITERATIONS is 1, the functionality is the same as exponential smoothing.
template <class T, int _NUM_ITERATIONS = 5>
class Animated {
public:

  static const int NUM_ITERATIONS = _NUM_ITERATIONS;

  Animated() : m_First(true), m_TargetFramerate(100.0f), m_SmoothStrength(0.8f) { }

  // const getters
  operator T() const { return Value(); }
  const T& Value() const { return m_Values[NUM_ITERATIONS-1]; }

  // setters to control animation
  void SetGoal(const T& goal) { m_Goal = goal; }
  void SetSmoothStrength(float smooth) { m_SmoothStrength = smooth; }

  // main update function, must be called every frame
  void Update(float deltaTime) {
    if (m_First || deltaTime < 0) {
      for (int i=0; i<NUM_ITERATIONS; i++) {
        m_Values[i] = m_Goal;
      }
      m_First = false;
    } else {
      const float dtExponent = deltaTime * m_TargetFramerate;
      const float smooth = std::pow(m_SmoothStrength, dtExponent);
      assert(smooth >= 0.0f && smooth <= 1.0f);
      for (int i=0; i<NUM_ITERATIONS; i++) {
        const T& prev = i == 0 ? m_Goal : m_Values[i-1];
        m_Values[i] = smooth*m_Values[i] + (1.0f-smooth)*prev;
      }
    }
  }

private:

  bool m_First;
  T m_Values[NUM_ITERATIONS];
  T m_Goal;
  float m_TargetFramerate;
  float m_SmoothStrength;
};
