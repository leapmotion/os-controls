#pragma once
#include <cassert>
#include <cmath>

#include <functional>
#include <stdexcept>

//A nice namespace to collect the easing functions, with a single default implementation to start it off.
namespace EasingFunctions{
  template<typename T>
  void Linear(T& current, const T& start, const T& goal, double percent) {
    current = start + (goal-start)*percent;
  }

  template<typename T>
  void QuadInOut(T& current, const T& start, const T& goal, double percent) {
    double t = percent; //current time
    double d = 1.0f; //overall duration
    T c = goal - start; //total movement
    T b = start;

    //Taken from http://gizma.com/easing/
    if ((t /= d / 2) < 1)
      current = static_cast<T>(((c / 2)*(t*t)) + b);
    else
      current = static_cast<T>(-c / 2 * (((t - 2)*(--t)) - 1) + b);
  }
}

/// A class for animated parameters.
/// Accepts an easing function, the default one being a simple linear easing.
/// The importaint feature is that you can precicely control how long it will take
/// the variable to reach it's goal, and once the goal is set all you have to do
/// is call the update function.  Setting a new goal while the animation is in progress
/// will cause completion to reset to 0, and the value at the time of setting to be the
/// new start value.  This makes it unsuitable for chasing behaviors where Set is called
/// often, however it makes it great for fire and forget animations where you want precice
/// control of the behavior.
template <class T>
class Animated{
public:
  typedef std::function<void(T& current, const T& start, const T& goal, double percent)> EasingFunction;

  Animated(const EasingFunction& func = EasingFunctions::Linear<T>) : Animated(T()) {}
  Animated(const T& initial,const EasingFunction& func = EasingFunctions::Linear<T>) :
    m_current(initial), m_start(initial), m_goal(initial), 
    m_duration(1.0), m_completion(0.0), m_easing(func)
  {}

  //If a SetDuration function is added, make sure you handle the implied change to m_completion!
  const double& Duration() const { return m_duration; }

  const T& Current() const { return m_current; }
  const T& Goal() const { return m_goal; }

  void SetEasingFunction(const EasingFunction& func) {
    m_easing = func;
  }

  void Set(const T& newGoal) {
    m_goal = newGoal;
    m_start = m_current;
    m_completion = 0;
  }

  void Set(const T& newGoal, double newDuration) {
    Set(newGoal);
    m_duration = newDuration;
  }

  void SetImmediate(const T& newGoal) {
    m_goal = newGoal;
    m_start = newGoal;
    m_current = newGoal;
    m_completion = 1.0f;
  }

  void Update(double deltaT) {
    if (!m_easing)
      throw std::runtime_error("No easing function defined");

    m_completion += deltaT / m_duration;
    m_completion = std::max(0.0, std::min(1.0, m_completion));

    m_easing(m_current, m_start, m_goal, m_completion);
  }

private:
  T m_current;
  T m_start; //I'd really like to figure out a way to not need this.
  T m_goal;

  double m_duration;
  double m_completion; ///% complete, a value between 0.0 and 1.0.

  EasingFunction m_easing;
};
