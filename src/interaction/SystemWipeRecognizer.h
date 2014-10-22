#pragma once

#include "Leap.h"

#include <autowiring/Autowired.h>

namespace Internal {

template <typename OwnerClass_, typename Event_, Event_ ENTER_, Event_ EXIT_>
class StateMachine {
public:

    typedef void (OwnerClass_::*StateMachineHandler)(Event_);

    StateMachine (OwnerClass_ &owner, StateMachineHandler initial_state)
        : m_owner(owner)
        , m_current_state(initial_state)
        , m_requested_transition_state(m_current_state)
    { }

    void SetTransitionRequest (StateMachineHandler s) { m_requested_transition_state = s; }

    void Initialize () {
        (m_owner.*m_current_state)(ENTER_);
    }
    void Run (Event_ event) {
        ProcessExitAndEnterEvents();
        (m_owner.*m_current_state)(event);
        ProcessExitAndEnterEvents();
    }
    void Shutdown () {
        (m_owner.*m_current_state)(EXIT_);
    }

private:

    void ProcessExitAndEnterEvents () {
        while (m_requested_transition_state != m_current_state) {
            (m_owner.*m_current_state)(EXIT_);
            m_current_state = m_requested_transition_state;
            (m_owner.*m_current_state)(ENTER_);
        }
    }

    OwnerClass_ &m_owner;
    StateMachineHandler m_current_state;
    StateMachineHandler m_requested_transition_state;
};

template <typename T_>
class Mipmap {
public:

    Mipmap () { }
    Mipmap (const std::vector<T_> &intensity) { Populate(intensity); }

    bool IsInitialized () const { return m_level.size() > 0; }
    // TODO: linear interpolation in both parameters.
    const T_ &Sample (size_t at_level, const T_ &param) const {
        if (param < 0.0f || param > 1.0f) {
            throw std::invalid_argument("param out of range [0,1]");
        }
        // This call will throw if at_level is out of range.
        const std::vector<T_> &level = m_level.at(at_level);
        // Map the range [0,1] onto [0,level.size()-1].
        assert(level.size() > 0);
        return level[static_cast<size_t>(param*(level.size()-1))];
    }

    void Populate (const std::vector<T_> &intensity) {
        m_level.clear();
        // IMPORTANT: Reserve all the space we'll need in m_level so that pointers
        // to elements of m_level stay valid for the duration of this computation.
        m_level.reserve(std::ceil(1.0f+std::log2(intensity.size())));
        m_level.emplace_back(intensity);
        // Construct all the levels of the mipmap via downsampling.
        const std::vector<T_> *previous = &m_level.back();
        while (previous->size()/2 > 0) {
            m_level.emplace_back();
            std::vector<T_> &next = m_level.back();
            Downsample(*previous, next);
            assert(next.size() > 0);
            previous = &next;
        }
    }

private:

    // Populate out with a half-sized downsampled version of in.
    static void Downsample (const std::vector<T_> &in, std::vector<T_> &out) {
        out.clear();
        for (size_t y = 0; y+1 < in.size(); y += 2) {
            out.push_back((in[y]+in[y+1])/2);
        }
    };

    std::vector<std::vector<T_>> m_level;
};

} // end of namespace Internal

struct SystemWipe {
  enum class Direction { UP, DOWN };

  bool isWiping;
  Direction direction;
};

class SystemWipeRecognizer {
public:

  SystemWipeRecognizer ();
  ~SystemWipeRecognizer ();

  void AutoFilter(const Leap::Frame& frame, SystemWipe& systemWipe);

private:

  enum class StateMachineEvent { ENTER, EXIT, FRAME };

  void WaitingForMassSignal (StateMachineEvent);
  void WaitingForHigherMassSignal (StateMachineEvent);
  void WaitingForCentroidDelta (StateMachineEvent);
  void Timeout (StateMachineEvent);

  Internal::StateMachine<SystemWipeRecognizer,StateMachineEvent,StateMachineEvent::ENTER,StateMachineEvent::EXIT> m_state_machine;
  double m_current_time;
  std::vector<float> m_brightness;
  double m_centroid_signal_start_time;
  double m_timeout_end_time;
  float m_centroid_start_value;
  float m_last_good_centroid_value;
  float m_current_centroid;
  float m_current_mass;
  SystemWipe *m_system_wipe;
};
