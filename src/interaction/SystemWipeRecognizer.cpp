#include "SystemWipeRecognizer.h"

#include <cmath>
#include <cstdint>
#include <vector>

#include <iostream> // TEMP
#include <iomanip> // TEMP

#define FORMAT_VALUE(x) #x << " = " << (x)

namespace Internal {

template <typename T_, typename IndexType_ = size_t>
class Linterp {
public:

  Linterp (T_ start, T_ end, IndexType_ count)
    : m_start(start)
    , m_end(end)
    , m_count_minus_one(count-1)
    , m(0)
  {
    if (count < 2) {
      throw std::invalid_argument("count must be at least 2.");
    }
  }

  operator T_ () const { return Value(); }
  T_ Value () const {
    IndexType_ one_minus_m = m_count_minus_one - m;
    return (one_minus_m*m_start + m*m_end)/m_count_minus_one;
  }
  const T_ &Start () const { return m_start; }
  const T_ &End () const { return m_end; }
  const T_ &LowerBound () const { return (m_start < m_end) ? m_start : m_end; }
  const T_ &UpperBound () const { return (m_start >= m_end) ? m_start : m_end; }

  bool IsNotAtEnd () const { return m <= m_count_minus_one; }
  void operator ++ () { ++m; }

private:

  // The ordered endpoints of the interpolation.
  T_ m_start;
  T_ m_end;
  // Derived from the number of samples to interpolate over.
  IndexType_ m_count_minus_one;
  // Defines a value in the range [0,1], stored using the range [0,COUNT_-1].
  IndexType_ m;
};

template <typename T_>
T_ Average (const std::vector<T_> &v) {
    // The average of no elements is defined by convention to be zero.
    if (v.size() == 0) {
        return T_(0);
    }
    T_ weighted_average(0);
    for (auto x : v) {
        weighted_average += x;
    }
    return weighted_average / v.size();
}

} // end of namespace Internal

SystemWipeRecognizer::SystemWipeRecognizer ()
  : m_state_machine(*this, &SystemWipeRecognizer::WaitingForMassSignal)
  , m_current_time(0.0)
{
  m_state_machine.Initialize();
}

SystemWipeRecognizer::~SystemWipeRecognizer () {
  m_state_machine.Shutdown();
}

void SystemWipeRecognizer::AutoFilter(const Leap::Frame& frame, SystemWipe& systemWipe) {
  systemWipe.isWiping = false;
  m_system_wipe = &systemWipe;

  m_current_time = 1.0e-6 * frame.timestamp();

  Leap::ImageList images = frame.images();
  // std::cerr << "number of images = " << images.count() << '\n';
  if (images.count() < 2) {
      return;
  }

  if (!(images[0].isValid() && images[1].isValid())) {
      return;
  }

    // Sample each of the images along vertical lines
    // std::vector<float> brightness;
    {
        assert(images[0].width() == images[1].width());
        assert(images[0].height() == images[1].height());
        size_t width = images[0].width();
        size_t height = images[0].height();
        assert(width > 0);
        assert(height > 0);
        m_brightness.clear();
        m_brightness.reserve(height);
        auto data0 = images[0].data();
        auto data1 = images[1].data();
        // Number of vertical lines to use
        static const size_t HORIZONTAL_SAMPLE_COUNT = 10;
        std::vector<float> row;
        row.reserve(HORIZONTAL_SAMPLE_COUNT);
        for (size_t y = 0; y < height; ++y) {
            row.clear();
            for (Internal::Linterp<size_t> x(0, width-1, HORIZONTAL_SAMPLE_COUNT); x.IsNotAtEnd(); ++x) {
                size_t data_index = y*width + x;
                row.push_back(std::max(data0[data_index],data1[data_index]) / 255.0f);
            }
            m_brightness.push_back(Internal::Average(row));
        }
    }
  // // Sample each of the images along vertical lines
  // {
  //     assert(images[0].width() == images[1].width());
  //     assert(images[0].height() == images[1].height());
  //     size_t width = images[0].width();
  //     size_t height = images[0].height();
  //     assert(width > 0);
  //     assert(height > 0);
  //     m_brightness.reserve(height);
  //     auto data0 = images[0].data();
  //     auto data1 = images[1].data();
  //     // Number of vertical lines to use
  //     static const size_t HORIZONTAL_SAMPLE_COUNT = 10;
  //     float row_weighted_average;
  //     for (size_t y = 0; y < height; ++y) {
  //         row_weighted_average = 0.0f;
  //         for (Internal::Linterp<size_t> x(0, width-1, HORIZONTAL_SAMPLE_COUNT); x.IsNotAtEnd(); ++x) {
  //             size_t data_index = y*width + x;
  //             row_weighted_average += std::max(data0[data_index],data1[data_index]) / 255.0f;
  //         }
  //         m_brightness.push_back(row_weighted_average / HORIZONTAL_SAMPLE_COUNT);
  //     }
  // }

  // Use a 6th order polynomial to approximate the maximum brightness curve for the
  // center vertical line for the camera.  This is used to normalize the brightness
  // values thereby accounting for the non-uniform LED illumination.
  // TODO: Maybe make a calibration tool in C++ so that these values could be
  // automatically determined again (this was determined in Sage math system).
  auto max_brightness_approximation = [](float t) {
      assert(t >= 0.0f && t <= 1.0f);
      static const size_t POLYNOMIAL_ORDER = 6;
      static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = {
        0.509999999995f,
        2.08500000785f,
        -2.29200009331f,
        -5.44499960929f,
        17.5499992571f,
        -17.819999345f,
        5.83199978266f
      };
      float power_of_t = 1.0f;
      float retval = 0.0f;
      for (size_t i = 0; i < POLYNOMIAL_ORDER+1; ++i) {
          retval += POLYNOMIAL_COEFFICIENTS[i]*power_of_t;
          power_of_t *= t;
      }
      return retval;
  };

  // m_current_centroid = 0.0f;
  // m_current_mass = 0.0f;
  // Internal::Linterp<float> t(0.0f, 1.0f, m_brightness.size());
  // for (auto b : m_brightness) {
  //   b /= max_brightness_approximation(t);
  //   // float s = b;
  //   float s = (b > 0.8f) ? 1 : 0;
  //   m_current_mass += s;
  //   m_current_centroid += s*t;
  // }
  // if (m_current_mass > std::numeric_limits<float>::epsilon()) {
  //   m_current_centroid /= m_current_mass; // Divide centroid through by mass before adjusting mass by sample_count.
  // }
  // m_current_mass /= m_brightness.size();



  Internal::Mipmap<float> mipmap(m_brightness);

  // Number of samples uniformly distributed.  TODO: compute which mipmap level to use based on this.
  m_current_mass = 0.0f;
  m_current_centroid = 0.0f;
  static const size_t SAMPLE_COUNT = 10;
  // for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
  for (Internal::Linterp<float> t(0.0, 1.0f, SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
    // float t = float(i) / (SAMPLE_COUNT-1);
    float b = mipmap.Sample(3,t);
    b /= max_brightness_approximation(t);
    // float s = b;
    float s = (b > 0.8f) ? 1 : 0;
    m_current_mass += s;
    m_current_centroid += s*t;
  }
  if (m_current_mass > std::numeric_limits<float>::epsilon()) {
    m_current_centroid /= m_current_mass; // Divide centroid through by mass before adjusting mass by sample_count.
  }
  m_current_mass /= SAMPLE_COUNT;

  auto percent = [](float b) -> int { return int(100.0f*b); };

  // // for (Internal::Linterp<float> t(0.0, 1.0f, SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
  // for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
  //   float t = float(i) / (SAMPLE_COUNT-1);
  //   float b = mipmap.Sample(3,t) / max_brightness_approximation(t);
  //   // std::cerr << std::setw(3) << filtered_value(b) << ',';
  //   std::cerr << (percent(m_current_mass) > 0 && i == size_t(std::round(m_current_centroid*(SAMPLE_COUNT-1))) ? 'X' : '.');
  // }
  // std::cerr << ", " << std::setw(3) << percent(m_current_mass) << "  " << std::setw(3) << percent(m_current_centroid) << '\n';



  m_state_machine.Run(StateMachineEvent::FRAME);
}

#define SET_TRANSITION_REQUEST_AND_RETURN(x) \
  do { \
    m_state_machine.SetTransitionRequest(&SystemWipeRecognizer::x); \
    return; \
  } while (false)

#define PRINT_STATEMACHINE_INFO(state_machine_name,event) \
  do { \
    if (event == StateMachineEvent::ENTER) { \
      std::cerr << '\n' << #state_machine_name << " - ENTER\n"; \
    } else if (event == StateMachineEvent::EXIT) { \
      std::cerr << #state_machine_name << " - EXIT\n\n"; \
    } \
  } while (false)

void SystemWipeRecognizer::WaitingForMassSignal (StateMachineEvent event) {
    PRINT_STATEMACHINE_INFO(WaitingForMassSignal,event);
    switch (event) {
        case StateMachineEvent::FRAME:
            if (m_current_mass >= 0.1f) {
                SET_TRANSITION_REQUEST_AND_RETURN(WaitingForHigherMassSignal);
            }
        default: return;
    }
}

void SystemWipeRecognizer::WaitingForHigherMassSignal (StateMachineEvent event) {
    PRINT_STATEMACHINE_INFO(WaitingForHigherMassSignal,event);
    switch (event) {
        case StateMachineEvent::ENTER:
            m_centroid_signal_start_time = m_current_time;
            m_centroid_start_value = m_current_centroid;
            m_last_good_centroid_value = m_centroid_start_value;
            std::cerr << FORMAT_VALUE(m_centroid_start_value) << "  \n";
            return;
        case StateMachineEvent::FRAME:
            if (m_current_mass >= 0.3f) {
                SET_TRANSITION_REQUEST_AND_RETURN(WaitingForCentroidDelta);
            }
            if (m_current_mass < 0.1f) {
                SET_TRANSITION_REQUEST_AND_RETURN(WaitingForMassSignal);
            }
            return;
        default: return;
    }
}

void SystemWipeRecognizer::WaitingForCentroidDelta (StateMachineEvent event) {
    PRINT_STATEMACHINE_INFO(WaitingForCentroidDelta,event);
    switch (event) {
        case StateMachineEvent::FRAME: {
            if (m_current_mass < 0.1f) {
                float delta = m_last_good_centroid_value - m_centroid_start_value;
                std::cerr << FORMAT_VALUE(m_last_good_centroid_value) << ", " << FORMAT_VALUE(m_centroid_start_value) << ", " << FORMAT_VALUE(delta) << '\n';
                if (std::abs(delta) > 0.4f) {
                    std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! centroid delta recognized -- " << (delta > 0.0f ? "DOWN" : "UP") << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                    m_system_wipe->isWiping = true;
                    m_system_wipe->direction = delta > 0.0f ? SystemWipe::Direction::UP : SystemWipe::Direction::DOWN;
                    m_timeout_end_time = m_current_time + 0.3;
                    SET_TRANSITION_REQUEST_AND_RETURN(Timeout);
                }
                SET_TRANSITION_REQUEST_AND_RETURN(WaitingForMassSignal);
            } else {
                m_last_good_centroid_value = m_current_centroid;
            }
            return;
        }
        default: return;
    }
}

void SystemWipeRecognizer::Timeout (StateMachineEvent event) {
    PRINT_STATEMACHINE_INFO(Timeout,event);
    switch (event) {
        case StateMachineEvent::FRAME:
            if (m_current_time >= m_timeout_end_time) {
                SET_TRANSITION_REQUEST_AND_RETURN(WaitingForMassSignal);
            }
        default: return;
    }
}
