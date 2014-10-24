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

  Linterp (T_ start, T_ end, IndexType_ sample_count)
    : m_start(start)
    , m_end(end)
    , m_sample_count_minus_one(sample_count-1)
    , m_index(0)
  {
    if (sample_count < 2) {
      throw std::invalid_argument("sample_count must be at least 2.");
    }
  }

  operator T_ () const { return Value(); }
  T_ Value () const {
    IndexType_ index_complement = m_sample_count_minus_one - m_index;
    return (index_complement*m_start + m_index*m_end)/m_sample_count_minus_one;
  }
  const IndexType_ &Index () const { return m_index; }
  const T_ &Start () const { return m_start; }
  const T_ &End () const { return m_end; }
  const T_ &LowerBound () const { return (m_start < m_end) ? m_start : m_end; }
  const T_ &UpperBound () const { return (m_start >= m_end) ? m_start : m_end; }

  bool IsNotAtEnd () const { return m_index <= m_sample_count_minus_one; }
  void operator ++ () { ++m_index; }

private:

  // The ordered endpoints of the interpolation.
  T_ m_start;
  T_ m_end;
  // Derived from the number of samples to interpolate over.
  IndexType_ m_sample_count_minus_one;
  // Defines a value in the range [0,1], stored using the range [0,COUNT_-1].
  IndexType_ m_index;
};

} // end of namespace Internal

// Tuning parameters
//const size_t SystemWipeRecognizer::SAMPLE_COUNT = 10;
const float SystemWipeRecognizer::BRIGHTNESS_ACTIVATION_THRESHOLD = 0.8f; // 0.8f works (value in original test)

SystemWipeRecognizer::SystemWipeRecognizer ()
  : m_state_machine(*this, &SystemWipeRecognizer::WaitingForMassSignal)
  , m_current_time(0.0)
{
  for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
    m_max_downsampled_brightness[i] = 0.0f;
  }
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

    if (false) {
      // compute average of row samples
      for (size_t y = 0; y < height; ++y) {
        float row_weighted_average = 0.0f;
        for (Internal::Linterp<size_t> x(0, width-1, HORIZONTAL_SAMPLE_COUNT); x.IsNotAtEnd(); ++x) {
          size_t data_index = y*width + x;
          row_weighted_average += std::max(data0[data_index],data1[data_index]) / 255.0f;
        }
        m_brightness.push_back(row_weighted_average / HORIZONTAL_SAMPLE_COUNT);
      }
    } else {
      // compute max of row samples
      for (size_t y = 0; y < height; ++y) {
        float row_max = 0.0f;
        for (Internal::Linterp<size_t> x(0, width-1, HORIZONTAL_SAMPLE_COUNT); x.IsNotAtEnd(); ++x) {
          size_t data_index = y*width + x;
          float brightness = std::max(data0[data_index],data1[data_index]) / 255.0f;
          if (brightness > row_max) {
            row_max = brightness;
          }
        }
        m_brightness.push_back(row_max);
      }
    }
  }

  // Use a 6th order polynomial to approximate the maximum brightness curve for the
  // center vertical line for the camera.  This is used to normalize the brightness
  // values thereby accounting for the non-uniform LED illumination.
  // TODO: Maybe make a calibration tool in C++ so that these values could be
  // automatically determined again (this was determined in Sage math system).
  auto max_brightness_approximation = [](float t) {
      assert(t >= 0.0f && t <= 1.0f);
      static const size_t POLYNOMIAL_ORDER = 6;
      static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = { 0.759765734259f, 1.99866765911f, -6.85648009151f, 11.15012094f, -8.86891604236f, 3.62432179126f, -1.107168802f };
      float power_of_t = 1.0f;
      float retval = 0.0f;
      for (size_t i = 0; i < POLYNOMIAL_ORDER+1; ++i) {
          retval += POLYNOMIAL_COEFFICIENTS[i]*power_of_t;
          power_of_t *= t;
      }
      return retval;
  };

  // Populate m_downsampled_brightness.
  {
    assert(m_brightness.size() > SAMPLE_COUNT);
    // std::cerr << FORMAT_VALUE(m_brightness.size()) << '\n';
    Internal::Linterp<size_t> i(0, m_brightness.size(), SAMPLE_COUNT+1);
    Internal::Linterp<size_t> next_i(i);
    ++next_i;
    Internal::Linterp<float> t(0.0, 1.0f, SAMPLE_COUNT);
    for ( ; t.IsNotAtEnd(); ++t, ++i, ++next_i) {
      float &b = m_downsampled_brightness[t.Index()];
      b = 0.0f;
      for (size_t y = i; y < next_i; ++y) {
        b += m_brightness[y];
      }
      // std::cerr << "    " << FORMAT_VALUE(t.Index()) << ", " << FORMAT_VALUE(next_i) << ", " << FORMAT_VALUE(i) << ", " << FORMAT_VALUE(next_i - i) << ", " << FORMAT_VALUE(b) << '\n';
      b /= next_i - i; // divide through by the number of summed up samples to get average
    }
  }

  // Update m_max_downsampled_brightness.
  for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
    if (m_downsampled_brightness[i] > m_max_downsampled_brightness[i]) {
      m_max_downsampled_brightness[i] = m_downsampled_brightness[i];
    }
  }

  // Number of samples uniformly distributed.  TODO: compute which mipmap level to use based on this.
  m_current_mass = 0.0f;
  m_current_centroid = 0.0f;
  for (Internal::Linterp<float> t(0.0, 1.0f, SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
    float b = m_downsampled_brightness[t.Index()];
    // b /= max_brightness_approximation(t);
    // float s = b;
    float s = (b > BRIGHTNESS_ACTIVATION_THRESHOLD) ? 1 : 0;
    m_current_mass += s;
    m_current_centroid += s*t;
  }
  if (m_current_mass > std::numeric_limits<float>::epsilon()) {
    m_current_centroid /= m_current_mass; // Divide centroid through by mass before adjusting mass by sample_count.
  }
  m_current_mass /= SAMPLE_COUNT;

  auto percent = [](float b) -> int { return int(100.0f*b); };

  for (Internal::Linterp<float> t(0.0, 1.0f, SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
    float b = m_downsampled_brightness[t.Index()];
    std::cerr << std::setw(3) << percent(b) << ',';
  }
  std::cerr << "  ";
  for (Internal::Linterp<float> t(0.0, 1.0f, SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
    float b = m_max_downsampled_brightness[t.Index()];
    std::cerr << std::setw(3) << percent(b) << ',';
  }
  std::cerr << "  ";
  for (Internal::Linterp<float> t(0.0, 1.0f, SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
    std::cerr << (percent(m_current_mass) > 0 && t.Index() == size_t(std::round(m_current_centroid*(SAMPLE_COUNT-1))) ? 'X' : '.');
  }
  std::cerr << ", " << std::setw(3) << percent(m_current_mass) << "  " << std::setw(3) << percent(m_current_centroid) << '\n';

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
    // PRINT_STATEMACHINE_INFO(WaitingForMassSignal,event);
    switch (event) {
        case StateMachineEvent::FRAME:
            if (m_current_mass >= 0.1f) {
                SET_TRANSITION_REQUEST_AND_RETURN(WaitingForHigherMassSignal);
            }
        default: return;
    }
}

void SystemWipeRecognizer::WaitingForHigherMassSignal (StateMachineEvent event) {
    // PRINT_STATEMACHINE_INFO(WaitingForHigherMassSignal,event);
    switch (event) {
        case StateMachineEvent::ENTER:
            m_centroid_signal_start_time = m_current_time;
            m_centroid_start_value = m_current_centroid;
            m_last_good_centroid_value = m_centroid_start_value;
            // std::cerr << FORMAT_VALUE(m_centroid_start_value) << "  \n";
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
    // PRINT_STATEMACHINE_INFO(WaitingForCentroidDelta,event);
    switch (event) {
        case StateMachineEvent::FRAME: {
            if (m_current_mass < 0.1f) {
                float delta = m_last_good_centroid_value - m_centroid_start_value;
                // std::cerr << FORMAT_VALUE(m_last_good_centroid_value) << ", " << FORMAT_VALUE(m_centroid_start_value) << ", " << FORMAT_VALUE(delta) << '\n';
                if (std::abs(delta) > 0.4f) {
                    // std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! centroid delta recognized -- " << (delta > 0.0f ? "DOWN" : "UP") << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
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
    // PRINT_STATEMACHINE_INFO(Timeout,event);
    switch (event) {
        case StateMachineEvent::FRAME:
            if (m_current_time >= m_timeout_end_time) {
                SET_TRANSITION_REQUEST_AND_RETURN(WaitingForMassSignal);
            }
        default: return;
    }
}
