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

// sample must live at least as long as the std::function returned by this function.
template <typename T_>
std::function<T_(T_)> LinearlyInterpolatedFunction (T_ start, T_ end, const std::vector<T_> &sample) {
  T_ domain_length = end - start;
  if (sample.size() == 0) {
    throw std::invalid_argument("Must provide a positive number of samples for function.");
  } else if (sample.size() == 1) {
    if (domain_length != T_(0)) {
      throw std::invalid_argument("If only one sample is given for function, then start must equal end.");
    }
  } else {
    if (domain_length <= T_(0)) {
      throw std::invalid_argument("If there are at least 2 samples for function, then start must be less than end.");
    }
  }
  return [start, domain_length, &sample](T_ param) -> T_ {
    assert(sample.size() > 0);
    if (sample.size() == 1) {
      return sample[0];
    }
    param -= start;
    param /= domain_length;
    if (param < T_(0) || param > T_(1)) {
      throw std::domain_error("param out of range (must be between start and end)");
    }
    if (param == T_(1)) {
      return sample.back();
    }
    T_ fractional_index = param*(sample.size()-1);
    size_t base_index = size_t(std::floor(fractional_index));
    assert(base_index < sample.size()-1);
    T_ fraction = std::fmod(fractional_index, T_(1));
    // Linearly interpolate between the adjacent samples.
    return sample[base_index]*(T_(1)-fraction) + sample[base_index+1]*fraction;
  };
}

} // end of namespace Internal

// Tuning parameters
const float SystemWipeRecognizer::PROPORTION_OF_IMAGE_HEIGHT_TO_USE = 0.75f;
const float SystemWipeRecognizer::BRIGHTNESS_ACTIVATION_THRESHOLD = 0.8f; // 0.8f works (value in original test)

SystemWipeRecognizer::SystemWipeRecognizer ()
  : m_state_machine(*this, &SystemWipeRecognizer::WaitingForAnyMassSignal)
  , m_current_time(0.0)
  , m_signal_history(2) // Only need 2 samples -- current and previous.
{
  m_state_machine.Start();

  // Populate the signal history with at least 2 samples so that we don't have to wait to access the history.
  m_signal_history.RecordSample(0.0f, 0.0f);
  m_signal_history.RecordSample(0.0f, 0.0f);
  // These asserts are here mainly to ensure that the accessors didn't throw due to lack of samples.
  assert(CurrentSignal() == Signal<float>(0.0f, 0.0f));
  assert(CurrentSignalDelta() == Signal<float>(0.0f, 0.0f));
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

  // Number of samples uniformly distributed.  TODO: compute which mipmap level to use based on this.
  {
    float centroid = 0.0f;
    float mass = 0.0f;
    for (Internal::Linterp<float> t(0.0f, 1.0f, SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
      float b = NormalizedBrightness(t);
      // float b = m_downsampled_brightness[t.Index()];
      // float s = b;
      float s = (b > BRIGHTNESS_ACTIVATION_THRESHOLD) ? 1 : 0;
      centroid += s*t;
      mass += s;
    }
    if (mass > std::numeric_limits<float>::epsilon()) {
      centroid /= mass; // Divide centroid through by mass before adjusting mass by sample_count.
    }
    mass /= SAMPLE_COUNT;
    m_signal_history.RecordSample(centroid, mass);
  }

  auto percent = [](float b) -> int { return int(100.0f*b); };

#define PRINT_IMAGE_PROCESSING_INFO 1

#if PRINT_IMAGE_PROCESSING_INFO
  if (true) {
    static const size_t PRINT_SAMPLE_COUNT = 22;
    // for (Internal::Linterp<float> t(0.0f, 1.0f, PRINT_SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
    //   float b = NormalizedBrightness(t);
    //   // float b = m_downsampled_brightness[t.Index()];
    //   std::cerr << std::setw(3) << percent(b) << ',';
    // }
    // std::cerr << "  ";
    for (Internal::Linterp<float> t(0.0, 1.0f, PRINT_SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
      float b = MeasuredMaxBrightness(t);
      std::cerr << std::setw(3) << percent(b) << ',';
    }
    size_t up_edge(std::round((PRINT_SAMPLE_COUNT-1)*CurrentSignal().UpEdge()));
    size_t down_edge(std::round((PRINT_SAMPLE_COUNT-1)*CurrentSignal().DownEdge()));
    size_t centroid(std::round((PRINT_SAMPLE_COUNT-1)*CurrentSignal().Centroid()));
    std::cerr << "  ";
    for (Internal::Linterp<float> t(0.0f, 1.0f, PRINT_SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
      // Print 3 characters to render:
      // - the up edge,
      // - the centroid.
      // - the down edge,
      // This produces an ascii-graphic indicator of the form
      // ......[ooooooooooooooooooooooooXoooooooooooooooooooooooo]...
      //    up_edge                 centroid                down_edge
      // Periods help indicate the extent of the graph, o indicates the
      // presence of an activated brightness sample.
      char underlying_char = NormalizedBrightness(t) > BRIGHTNESS_ACTIVATION_THRESHOLD ? 'o' : '.';
      // char underlying_char = m_downsampled_brightness[t.Index()] > BRIGHTNESS_ACTIVATION_THRESHOLD ? 'o' : '.';
      std::cerr << (t.Index() == up_edge  ? '[' : underlying_char);
      std::cerr << (t.Index() == centroid  ? 'X' : underlying_char);
      std::cerr << (t.Index() == down_edge ? ']' : underlying_char);
    }
    std::cerr << "  mass: " << std::setw(10) << CurrentSignal().Mass() << ", centroid = " << std::setw(10) << CurrentSignal().Centroid();
  }
#endif

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

  if (m_brightness.size() != end_y - begin_y) {
    m_brightness.reserve(end_y - begin_y);
    m_brightness.resize(end_y - begin_y);
    Brightness = Internal::LinearlyInterpolatedFunction(0.0f, 1.0f, m_brightness);
  }

  if (m_measured_max_brightness.size() != end_y - begin_y) {
    m_measured_max_brightness.reserve(end_y - begin_y);
    m_measured_max_brightness.clear();
    m_measured_max_brightness.resize(end_y - begin_y, 0.0f); // Reset all to zero.
    MeasuredMaxBrightness = Internal::LinearlyInterpolatedFunction(0.0f, 1.0f, m_measured_max_brightness);
  }

  // compute max of row samples
  size_t h_low = 0; //(width-1)/5;
  size_t h_high = width-1; //4*(width-1)/5;
  for (size_t y = begin_y; y < end_y; ++y) {
    uint8_t row_max = 0;
    for (Internal::Linterp<size_t> x(h_low, h_high, HORIZONTAL_SAMPLE_COUNT); x.IsNotAtEnd(); ++x) {
      size_t data_index = y*width + x;
      uint8_t brightness = std::max(data0[data_index],data1[data_index]);
      if (brightness > row_max) {
        row_max = brightness;
      }
    }
    float brightness = row_max / 255.0f;
    m_brightness[y - begin_y] = brightness; // Divide by 255.0f to normalize the range [0, 255] to [0.0f, 1.0f].
    if (brightness > m_measured_max_brightness[y - begin_y]) {
      m_measured_max_brightness[y - begin_y] = brightness;
    }
  }
}

float SystemWipeRecognizer::ModeledMaxBrightness (float t) {
  // Use a 6th order polynomial to approximate the maximum brightness curve for the
  // center vertical line for the camera.  This is used to normalize the brightness
  // values thereby accounting for the non-uniform LED illumination.
  // TODO: Maybe make a calibration tool in C++ so that these values could be
  // automatically determined again (this was determined in Sage math system).
  assert(t >= 0.0f && t <= 1.0f);
  static const size_t POLYNOMIAL_ORDER = 6;
  static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = { 0.538455f, 1.64043f, 10.0559f, -68.0226f, 145.586f, -133.89f, 44.6299f };
  float power_of_t = 1.0f;
  float retval = 0.0f;
  for (size_t i = 0; i < POLYNOMIAL_ORDER+1; ++i) {
    retval += POLYNOMIAL_COEFFICIENTS[i]*power_of_t;
    power_of_t *= t;
  }
  return retval;
}

float SystemWipeRecognizer::NormalizedBrightness (float t) {
  return Brightness(t) / ModeledMaxBrightness(t);
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
      if (CurrentSignal().Mass() >= 0.1f) {
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
      m_first_good_up_tracking_value = CurrentSignal().TrackingValue(SystemWipe::Direction::UP);
      m_first_good_down_tracking_value = CurrentSignal().TrackingValue(SystemWipe::Direction::DOWN);
      // Intentionally fall through so that the frame logic in this state is processed.
    case StateMachineEvent::FRAME: {
      // Determine if this will be an up or a down wipe.  An up-wipe (that's.... an unsavory term)
      // is defined to begin when the up edge is at the bottom of the image, poised to proceed
      // upward.  Analogously, a down-wipe is defined to begin when the down edge is at the top
      // of the image, poised to proceed downward.  These conditions are mutually exclusive by
      // construction.
      float up_tracking_value = CurrentSignal().TrackingValue(SystemWipe::Direction::UP);
      float down_tracking_value = CurrentSignal().TrackingValue(SystemWipe::Direction::DOWN);
      static const float WIPE_START_UPPER_BOUND = 0.5f;
      bool detected_sufficient_mass = CurrentSignal().Mass() >= 0.25f;
      bool up_edge_moving_up_from_bottom = up_tracking_value <= WIPE_START_UPPER_BOUND && CurrentSignalDelta().TrackingVelocity(SystemWipe::Direction::UP) >= 0.0f;
      bool down_edge_moving_down_from_top = down_tracking_value <= WIPE_START_UPPER_BOUND && CurrentSignalDelta().TrackingVelocity(SystemWipe::Direction::DOWN) >= 0.0f;
      bool detected_higher_mass_signal = detected_sufficient_mass && (up_edge_moving_up_from_bottom || down_edge_moving_down_from_top);
      if (detected_higher_mass_signal) {
        m_wipe_direction = down_tracking_value <= WIPE_START_UPPER_BOUND ? SystemWipe::Direction::DOWN : SystemWipe::Direction::UP;
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
      float begin_tracking_value = CurrentSignal().TrackingValue(m_wipe_direction);
      float complete_tracking_value = begin_tracking_value + 0.75f*(1.0f - begin_tracking_value); // 70% of the way to the other edge.
      ProgressTransform = [begin_tracking_value, complete_tracking_value](float tracking_value) {
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
      m_system_wipe->direction = m_wipe_direction;
      // There are three ways for the recognition to end:
      // 1. The mass dropping low enough (end without "gesture complete").
      // 2. The wipe gesture proceeding far enough across the screen to register (end with "gesture complete").
      // 3. The wipe regressing back across the screen enough to abort the gesture (end without "gesture complete").
      if (CurrentSignal().Mass() < 0.1f) { // Case 1.
        m_system_wipe->status = SystemWipe::Status::ABORT;
        m_system_wipe->progress = 0.0f;
        SET_TRANSITION_REQUEST_AND_RETURN(WaitingForAnyMassSignal);
      }
      m_system_wipe->status = SystemWipe::Status::UPDATE;
      m_system_wipe->progress = ProgressTransform(CurrentSignal().TrackingValue(m_wipe_direction));
      if (m_system_wipe->progress == 1.0f) { // Case 2.
        m_system_wipe->status = SystemWipe::Status::COMPLETE;
        SET_TRANSITION_REQUEST_AND_RETURN(Timeout);
      } // TODO: Case 3.
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
        SET_TRANSITION_REQUEST_AND_RETURN(WaitingForAnyMassSignal);
      }
    default: return;
  }
}
