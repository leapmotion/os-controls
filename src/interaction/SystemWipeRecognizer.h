#pragma once

#include "Leap.h"

#include <autowiring/Autowired.h>
#include <deque>
#include <functional>

namespace Internal {

template <typename OwnerClass_, typename Event_, Event_ ENTER_, Event_ EXIT_>
class StateMachine {
public:

  typedef void (OwnerClass_::*StateMachineHandler)(Event_);

  StateMachine (OwnerClass_ &owner, StateMachineHandler initial_state)
    : m_owner(owner)
    , m_initial_state(initial_state)
    , m_current_state(m_initial_state)
    , m_requested_transition_state(m_current_state)
  { }

  StateMachineHandler InitialState () const { return m_initial_state; }
  StateMachineHandler CurrentState () const { return m_current_state; }
  bool TransitionIsRequested () const { return m_requested_transition_state != m_current_state; }
  // There is only a requested transition if the requested transition state differs from the current state.
  StateMachineHandler RequestedTransitionState () const { return m_requested_transition_state; }

  void SetInitialState (StateMachineHandler s) { m_initial_state = s; }
  void SetTransitionRequest (StateMachineHandler s) { m_requested_transition_state = s; }

  // Sets the current state to the initial state and issues the ENTER_ event to it.
  void Start () {
    m_current_state = m_initial_state;
    m_requested_transition_state = m_current_state; // These states being the same indicates no requested transition.
    (m_owner.*m_current_state)(ENTER_);
  }
  // Runs the current state until there is no requested transition.
  void Run (Event_ event) {
    ProcessExitAndEnterEvents();
    (m_owner.*m_current_state)(event);
    ProcessExitAndEnterEvents();
  }
  // Issues the EXIT_ event to the current state.
  void Finish () {
    (m_owner.*m_current_state)(EXIT_);
  }

private:

  void ProcessExitAndEnterEvents () {
    while (TransitionIsRequested()) {
      (m_owner.*m_current_state)(EXIT_);
      m_current_state = m_requested_transition_state;
      (m_owner.*m_current_state)(ENTER_);
    }
  }

  OwnerClass_ &m_owner;
  StateMachineHandler m_initial_state;
  StateMachineHandler m_current_state;
  StateMachineHandler m_requested_transition_state;
};

template <typename Data_>
class History {
public:

  History (size_t max_sample_count) : m_max_sample_count(max_sample_count) { }

  size_t SampleCount () const { return m_data.size(); }
  size_t MaxSampleCount () const { return m_max_sample_count; }
  // historical_sample_index is how far back into history to go.  Specifying 0 gives the most recent sample.
  // An exception will be thrown if the index exceeds the number of samples.
  const Data_ &operator [] (size_t historical_sample_index) const {
    return m_data.at(historical_sample_index);
  }

  template <typename... Types_>
  void RecordSample (Types_&&... args) {
    assert(m_data.size() <= m_max_sample_count);
    if (m_data.size() >= m_max_sample_count) {
      m_data.pop_back();
    }
    m_data.emplace_front(args...);
  }

private:

  // Max number of samples that will be stored.
  size_t m_max_sample_count;
  // The front is the most recent sample.
  std::deque<Data_> m_data;
};

template <typename T_, typename... OtherParams_>
class PiecewiseLinearlyInterpolatedFunction : public std::vector<T_,OtherParams_...> {
public:

  PiecewiseLinearlyInterpolatedFunction (T_ start, T_ end) : m_start(start), m_end(end), m_domain_length(end - start) {
    if (m_domain_length <= T_(0)) {
      throw std::invalid_argument("start must be less than end -- must have a nonzero domain interval length.");
    }
  }

  const T_ &Start () const { return m_start; }
  const T_ &End () const { return m_end; }
  const T_ &DomainLength () const { return m_domain_length; }

  // Evaluate the function at the given parameter value.  param must be in the range [Start(), End()] (inclusive).
  // There must be at least one sample element in this object (regarded as a vector).  If there is exactly one
  // sample, then the function is defined to be constant, equal to that sample value.  Otherwise, the function
  // is defined to be a piecewise linear function, interpolated between the vector's elements, which are interpreted
  // as uniformly-spaced samples of the function this object represents.
  T_ operator () (T_ param) const {
    const std::vector<T_> &sample = *this;
    if (sample.size() == 0) {
      throw std::invalid_argument("Must provide a positive number of samples for function.");
    } else if (sample.size() == 1) {
      // If there is 1 sample, assume the function is constant.
      return sample[0];
    } else {
      param -= m_start;
      param /= m_domain_length;
      if (param < T_(0) || param > T_(1)) {
        throw std::domain_error("param out of range (must be between start and end)");
      }
      if (param == T_(1)) { // If param is exactly at the end, return the last sample.
        return sample.back();
      }
      // Otherwise, linearly interpolate adjacent samples.
      T_ fractional_index = param*(sample.size()-1);
      size_t base_index = size_t(std::floor(fractional_index));
      assert(base_index < sample.size()-1);
      T_ fraction = std::fmod(fractional_index, T_(1));
      return sample[base_index]*(T_(1)-fraction) + sample[base_index+1]*fraction;
    }
  }

private:

  T_ m_start;
  T_ m_end;
  T_ m_domain_length;
};

} // end of namespace Internal

struct SystemWipe {
  enum class Direction { UP, DOWN };
  enum class Status { NOT_ACTIVE, BEGIN, UPDATE, COMPLETE, ABORT };

  // Indicates if the gesture is not active, is beginning, is updating, or is complete.
  Status status;
  // Indicates if the gesture is moving up or down.  This value is undefined if status is NOT_ACTIVE.
  Direction direction;
  // A value in the range [0,1] indicating the real-time progress of the gesture.  This value
  // is undefined if status is NOT_ACTIVE.
  float progress;
};

class SystemWipeRecognizer {
public:

  SystemWipeRecognizer ();
  ~SystemWipeRecognizer ();

  void AutoFilter(const Leap::Frame& frame, SystemWipe& systemWipe);

private:

  template <typename T_>
  struct Signal {
    Signal () : m_centroid(0), m_mass(0) { }
    Signal (T_ centroid, T_ mass) : m_centroid(centroid), m_mass(mass) { }
    bool operator == (const Signal &other) const { return m_centroid == other.m_centroid && m_mass == other.m_mass; }
    const T_ &Centroid () const { return m_centroid; }
    const T_ &Mass () const { return m_mass; }
    T_ UpEdge () const { return m_centroid - T_(0.5)*m_mass; }
    T_ DownEdge () const { return m_centroid + T_(0.5)*m_mass; }
    T_ TrackingValue (SystemWipe::Direction d) const { return d == SystemWipe::Direction::UP ? T_(1)-UpEdge() : DownEdge(); }
    T_ TrackingVelocity (SystemWipe::Direction d) const { return d == SystemWipe::Direction::UP ? -UpEdge() : DownEdge(); }
    // This is here so that a discrete derivative is computable.
    Signal operator - (const Signal &rhs) const { return Signal(m_centroid-rhs.m_centroid, m_mass-rhs.m_mass); }
  private:
    T_ m_centroid;
    T_ m_mass;
  };

  // Populates m_brightness and updates m_measured_max_brightness.
  void ComputeBrightness (const Leap::ImageList &images);

#define LEAP_INTERNAL_MEASURE_MAX_BRIGHTNESS 0

  // Convenience accessors
  static float ModeledMaxBrightness (float t);
#if LEAP_INTERNAL_MEASURE_MAX_BRIGHTNESS
  float MeasuredMaxBrightness (float t) const { return m_measured_max_brightness(t); }
#endif
  float Brightness (float t) const { return m_brightness(t); }
  float NormalizedBrightness (float t);
  std::function<float(float)> ProgressTransform;
  const Signal<float> &CurrentSignal () const { return m_signal_history[0]; }
  Signal<float> CurrentSignalDelta () const { return m_signal_history[0] - m_signal_history[1]; }

  // Tuning parameters

  // Number of samples to analyze in the gesture detection.  [Vertical strip(s) of] The original image(s)
  // will be downsampled to match this number.
  static const size_t SAMPLE_COUNT = 500;// 30;

  // State machine related -- the methods are states.

  enum class StateMachineEvent { ENTER, EXIT, FRAME };

  void WaitingForAnyMassSignal (StateMachineEvent);
  void WaitingForMassActivationThreshold (StateMachineEvent);
  void RecognizingGesture (StateMachineEvent);
  void Timeout (StateMachineEvent);

  Internal::StateMachine<SystemWipeRecognizer,StateMachineEvent,StateMachineEvent::ENTER,StateMachineEvent::EXIT> m_state_machine;

  // Non-state-machine member variables.

  double m_current_time;
  Internal::PiecewiseLinearlyInterpolatedFunction<float> m_brightness;
#if LEAP_INTERNAL_MEASURE_MAX_BRIGHTNESS
  Internal::PiecewiseLinearlyInterpolatedFunction<float> m_measured_max_brightness;
#endif
  double m_centroid_signal_start_time;
  double m_timeout_end_time;
  float m_first_good_up_tracking_value;
  float m_first_good_down_tracking_value;
  float m_initial_tracking_value;
  Internal::History<Signal<float>> m_signal_history;
  SystemWipe *m_system_wipe;
  SystemWipe::Direction m_wipe_direction;
};
