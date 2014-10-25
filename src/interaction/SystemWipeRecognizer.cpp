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
const float SystemWipeRecognizer::PROPORTION_OF_IMAGE_HEIGHT_TO_USE = 0.75f;
const float SystemWipeRecognizer::BRIGHTNESS_ACTIVATION_THRESHOLD = 0.8f; // 0.8f works (value in original test)
const float SystemWipeRecognizer::WIPE_END_DELTA_THRESHOLD = 0.4f;

SystemWipeRecognizer::SystemWipeRecognizer ()
  : m_state_machine(*this, &SystemWipeRecognizer::WaitingForAnyMassSignal)
  , m_current_time(0.0)
{
  for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
    m_max_downsampled_brightness[i] = 0.0f;
  }
  m_state_machine.Start();
}

SystemWipeRecognizer::~SystemWipeRecognizer () {
  m_state_machine.Finish();
}

void SystemWipeRecognizer::AutoFilter(const Leap::Frame& frame, SystemWipe& systemWipe) {
  systemWipe.status = SystemWipe::Status::NOT_ACTIVE;
  m_system_wipe = &systemWipe;

  m_current_time = 1.0e-6 * frame.timestamp();

  if (frame.images().count() < 2 || !frame.images()[0].isValid() || !frame.images()[1].isValid()) {
      return;
  }

  ComputeBrightness(frame.images());
  ComputeDownsampledBrightness();

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

  if (true) {
    for (Internal::Linterp<float> t(0.0, 1.0f, SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
      float b = m_downsampled_brightness[t.Index()];
      std::cerr << std::setw(3) << percent(b) << ',';
    }
    // std::cerr << "  ";
    // for (Internal::Linterp<float> t(0.0, 1.0f, SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
    //   float b = m_max_downsampled_brightness[t.Index()];
    //   std::cerr << std::setw(3) << percent(b) << ',';
    // }
    size_t up_edge(std::round((SAMPLE_COUNT-1)*UpEdge()));
    size_t down_edge(std::round((SAMPLE_COUNT-1)*DownEdge()));
    size_t centroid(std::round((SAMPLE_COUNT-1)*m_current_centroid));
    std::cerr << "  ";
    for (Internal::Linterp<float> t(0.0, 1.0f, SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
      // Print 3 characters to render:
      // - the up edge,
      // - the centroid.
      // - the down edge,
      // This produces an ascii-graphic indicator of the form
      // ......[ooooooooooooooooooooooooXoooooooooooooooooooooooo]...
      //    up_edge                 centroid                down_edge
      // Periods help indicate the extent of the graph, o indicates the
      // presence of an activated brightness sample.
      char underlying_char = m_downsampled_brightness[t.Index()] > BRIGHTNESS_ACTIVATION_THRESHOLD ? 'o' : '.';
      std::cerr << (t.Index() == up_edge  ? '[' : underlying_char);
      std::cerr << (t.Index() == centroid  ? 'X' : underlying_char);
      std::cerr << (t.Index() == down_edge ? ']' : underlying_char);
    }
    std::cerr << ", " << std::setw(3) << percent(m_current_mass);// << '\n';
    // std::cerr << ", " << std::setw(3) << percent(m_current_mass) << "  " << std::setw(3) << percent(m_current_centroid) << '\n';
  }

  m_state_machine.Run(StateMachineEvent::FRAME);
}

void SystemWipeRecognizer::ComputeBrightness (const Leap::ImageList &images) {
  assert(images.count() >= 2 && images[0].isValid() && images[1].isValid());

  // Sample each of the images along vertical lines
  assert(images[0].width() == images[1].width());
  assert(images[0].height() == images[1].height());
  size_t width = images[0].width();
  size_t height = images[0].height();
  assert(width > 0);
  assert(height > 0);
  auto data0 = images[0].data();
  auto data1 = images[1].data();
  // Number of vertical lines to use
  static const size_t HORIZONTAL_SAMPLE_COUNT = 10;

  assert(0.0f <= PROPORTION_OF_IMAGE_HEIGHT_TO_USE && PROPORTION_OF_IMAGE_HEIGHT_TO_USE <= 1.0f);
  size_t begin_y = static_cast<size_t>(0.5f*(1.0f-PROPORTION_OF_IMAGE_HEIGHT_TO_USE)*height);
  size_t end_y = height - begin_y;

  m_brightness.clear();
  m_brightness.reserve(end_y - begin_y);

  // compute max of row samples
  for (size_t y = begin_y; y < end_y; ++y) {
    uint8_t row_max = 0;
    for (Internal::Linterp<size_t> x(0, width-1, HORIZONTAL_SAMPLE_COUNT); x.IsNotAtEnd(); ++x) {
      size_t data_index = y*width + x;
      uint8_t brightness = std::max(data0[data_index],data1[data_index]);
      if (brightness > row_max) {
        row_max = brightness;
      }
    }
    m_brightness.push_back(row_max/255.0f); // Divide by 255.0f to normalize the range [0, 255] to [0.0f, 1.0f].
  }
}

void SystemWipeRecognizer::ComputeDownsampledBrightness () {
  assert(m_brightness.size() >= SAMPLE_COUNT);
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
    b /= next_i - i; // divide through by the number of summed up samples to get average
  }
}

#define SET_TRANSITION_REQUEST_AND_RETURN(x) \
  do { \
    m_state_machine.SetTransitionRequest(&SystemWipeRecognizer::x); \
    return; \
  } while (false)

#define ENABLE_PRINT_STATEMACHINE_INFO 0

#if ENABLE_PRINT_STATEMACHINE_INFO
  #define PRINT_STATEMACHINE_INFO(state_machine_name,event) \
    do { \
      if (event == StateMachineEvent::ENTER) { \
        std::cerr << '\n' << #state_machine_name << " - ENTER\n"; \
      } else if (event == StateMachineEvent::EXIT) { \
        std::cerr << #state_machine_name << " - EXIT\n\n"; \
      } \
    } while (false)
#else
  #define PRINT_STATEMACHINE_INFO(state_machine_name,event)
#endif

void SystemWipeRecognizer::WaitingForAnyMassSignal (StateMachineEvent event) {
  PRINT_STATEMACHINE_INFO(WaitingForAnyMassSignal,event);
  switch (event) {
    case StateMachineEvent::FRAME:
      if (m_current_mass >= 0.1f) {
        SET_TRANSITION_REQUEST_AND_RETURN(WaitingForMassActivationThreshold);
      }
    default: return;
  }
}

void SystemWipeRecognizer::WaitingForMassActivationThreshold (StateMachineEvent event) {
  PRINT_STATEMACHINE_INFO(WaitingForMassActivationThreshold,event);
  switch (event) {
    case StateMachineEvent::ENTER:
      m_centroid_signal_start_time = m_current_time;
      m_first_good_up_tracking_value = TrackingValue(SystemWipe::Direction::UP);
      m_first_good_down_tracking_value = TrackingValue(SystemWipe::Direction::DOWN);
      // std::cerr << FORMAT_VALUE(m_centroid_start_value) << "  \n";
      // Intentionally fall through so that the frame logic in this state is processed.
    case StateMachineEvent::FRAME: {
      // Determine if this will be an up or a down wipe.  An up-wipe (that's.... an unsavory term)
      // is defined to begin when the up edge is at the bottom of the image, poised to proceed
      // upward.  Analogously, a down-wipe is defined to begin when the down edge is at the top
      // of the image, poised to proceed downward.  These conditions are mutually exclusive by
      // construction.
      float up_tracking_value = TrackingValue(SystemWipe::Direction::UP);
      float down_tracking_value = TrackingValue(SystemWipe::Direction::DOWN);
      // std::cerr << FORMAT_VALUE(up_tracking_value) << ", " << FORMAT_VALUE(down_tracking_value) << "  ";
      static const float WIPE_START_UPPER_BOUND = 0.5f; //0.8f - WIPE_END_DELTA_THRESHOLD;
      bool detected_higher_mass_signal = m_current_mass >= 0.25f && (up_tracking_value <= WIPE_START_UPPER_BOUND || down_tracking_value <= WIPE_START_UPPER_BOUND);
      if (detected_higher_mass_signal) {
        m_wipe_direction = down_tracking_value <= WIPE_START_UPPER_BOUND ? SystemWipe::Direction::DOWN : SystemWipe::Direction::UP;
        // float first_good_tracking_value = m_wipe_direction == SystemWipe::Direction::UP ? m_first_good_up_tracking_value : m_first_good_down_tracking_value;
        // m_initial_tracking_value = std::min(TrackingValue(m_wipe_direction), first_good_tracking_value);
        SET_TRANSITION_REQUEST_AND_RETURN(RecognizingGesture);
      }
    }
    default: return;
  }
}

void SystemWipeRecognizer::RecognizingGesture (StateMachineEvent event) {
  PRINT_STATEMACHINE_INFO(RecognizingGesture,event);
  switch (event) {
    case StateMachineEvent::ENTER: {
      // float first_good_tracking_value = m_wipe_direction == SystemWipe::Direction::UP ? m_first_good_up_tracking_value : m_first_good_down_tracking_value;
      // m_initial_tracking_value = std::min(TrackingValue(m_wipe_direction), first_good_tracking_value);
      // m_initial_tracking_value = TrackingValue(m_wipe_direction);
      float begin_tracking_value = TrackingValue(m_wipe_direction);
      float complete_tracking_value = begin_tracking_value + 0.75f*(1.0f - begin_tracking_value); // 70% of the way to the other edge.
      m_progress_transform = [begin_tracking_value, complete_tracking_value](float tracking_value) {
        // The range [begin_tracking_value, complete_tracking_value] shall map onto [0, 1].
        float progress = (tracking_value - begin_tracking_value) / (complete_tracking_value - begin_tracking_value);
        // Clamp to the range [0, 1].
        return std::min(std::max(0.0f, progress), 1.0f);
      };
      m_system_wipe->status = SystemWipe::Status::BEGIN;
      m_system_wipe->direction = m_wipe_direction;
      m_system_wipe->progress = 0.0f;
      return;
    }
    case StateMachineEvent::FRAME: {
      // There are three ways for the recognition to end:
      // 1. The mass dropping low enough (end without "gesture complete").
      // 2. The wipe gesture proceeding far enough across the screen to register (end with "gesture complete").
      // 3. The wipe regressing back across the screen enough to abort the gesture (end without "gesture complete").
      if (m_current_mass < 0.1f) { // Case 1.
        SET_TRANSITION_REQUEST_AND_RETURN(WaitingForAnyMassSignal);
      }
      // float tracking_value = TrackingValue(m_wipe_direction);
      // static const auto clamp = [](float x) { return std::min(std::max(0.0f, x), 1.0f); };
      m_system_wipe->status = SystemWipe::Status::UPDATE;
      m_system_wipe->direction = m_wipe_direction;
      m_system_wipe->progress = m_progress_transform(TrackingValue(m_wipe_direction)); //clamp((tracking_value - m_initial_tracking_value) / WIPE_END_DELTA_THRESHOLD);
      if (m_system_wipe->progress == 1.0f) { // Case 2.
        m_system_wipe->status = SystemWipe::Status::COMPLETE;
        SET_TRANSITION_REQUEST_AND_RETURN(Timeout);
      } // TODO: Case 3.
      return;
    }
    default: return;
  }
}

// void SystemWipeRecognizer::WaitingForSufficientDelta (StateMachineEvent event) {
//   PRINT_STATEMACHINE_INFO(WaitingForSufficientDelta,event);
//   switch (event) {
//     case StateMachineEvent::ENTER: // Process frame upon entry by falling through.
//     case StateMachineEvent::FRAME: {
//       // float delta = m_last_good_centroid_value - m_centroid_start_value;
//       // The up edge is tracking motion in the negative direction, so only record negative deltas.
//       float up_edge_delta = std::abs(std::min(0.0f, m_last_good_up_edge_value - m_up_edge_start_value));
//       // The down edge is tracking motion in the positive direction, so only record positive deltas.
//       float down_edge_delta = std::abs(std::max(0.0f, m_last_good_down_edge_value - m_down_edge_start_value));

//       // There are several ways for the gesture to end -- losing enough mass, or having
//       // a large enough delta on either the up edge or the down edge.
//       bool end_gesture = m_current_mass < 0.1f || up_edge_delta > WIPE_END_DELTA_THRESHOLD || down_edge_delta > WIPE_END_DELTA_THRESHOLD;      
//       if (end_gesture) {
//         // std::cerr << FORMAT_VALUE(m_last_good_centroid_value) << ", " << FORMAT_VALUE(m_centroid_start_value) << ", " << FORMAT_VALUE(delta) << '\n';
//         // std::cerr << FORMAT_VALUE(up_edge_delta) << ", " << FORMAT_VALUE(down_edge_delta) << '\n';
//         if (up_edge_delta > WIPE_END_DELTA_THRESHOLD) {
//           // std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! centroid delta recognized -- " << (delta > 0.0f ? "DOWN" : "UP") << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
//           m_system_wipe->isWiping = true;
//           m_system_wipe->direction = SystemWipe::Direction::UP; // The pixel indices go down in the image's up direction.
//           m_timeout_end_time = m_current_time + 0.3;
//           SET_TRANSITION_REQUEST_AND_RETURN(Timeout);
//         } else if (down_edge_delta > WIPE_END_DELTA_THRESHOLD) {
//           // std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! centroid delta recognized -- " << (delta > 0.0f ? "DOWN" : "UP") << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
//           m_system_wipe->isWiping = true;
//           m_system_wipe->direction = SystemWipe::Direction::DOWN; // The pixel indices go up in the image's down direction.
//           m_timeout_end_time = m_current_time + 0.3;
//           SET_TRANSITION_REQUEST_AND_RETURN(Timeout);
//         }
//         // Not wiping.
//         SET_TRANSITION_REQUEST_AND_RETURN(WaitingForAnyMassSignal);
//       } else {
//         m_last_good_up_edge_value = UpEdge();
//         m_last_good_centroid_value = m_current_centroid;
//         m_last_good_down_edge_value = DownEdge();
//       }
//       return;
//     }
//     default: return;
//   }
// }

void SystemWipeRecognizer::Timeout (StateMachineEvent event) {
  PRINT_STATEMACHINE_INFO(Timeout,event);
  switch (event) {
    case StateMachineEvent::FRAME:
      if (m_current_time >= m_timeout_end_time) {
        SET_TRANSITION_REQUEST_AND_RETURN(WaitingForAnyMassSignal);
      }
    default: return;
  }
}
