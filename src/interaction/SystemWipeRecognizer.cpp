#include "SystemWipeRecognizer.h"

#include <cmath>
#include <cstdint>
#include <vector>

#include <iostream> // TEMP
#include <iomanip> // TEMP

#define FORMAT_VALUE(x) #x << " = " << (x)

// Tuning parameters
static const float PROPORTION_OF_IMAGE_WIDTH_TO_USE = 0.9f;
static const float PROPORTION_OF_IMAGE_HEIGHT_TO_USE = 0.75f;
static const float BRIGHTNESS_ACTIVATION_THRESHOLD = 1.0f; // 0.8f; // 0.8f works (value in original test)
static const float BEGINNING_MASS_ACTIVATION_THRESHOLD = 0.3f; // 0.35f;
static const float PORTION_OF_REMAINDER_UNTIL_COMPLETE = 0.75f;
static const float TIMEOUT_DURATION = 0.5f;

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

SystemWipeRecognizer::SystemWipeRecognizer ()
  : m_state_machine(*this, &SystemWipeRecognizer::WaitingForAnyMassSignal)
  , m_current_time(0.0)
#if LEAP_INTERNAL_MEASURE_MAX_BRIGHTNESS
  , m_measured_max_brightness(0.0f, 1.0f) // This defines the interval over which the max brightness function is defined; [0,1].
#endif
  , m_brightness(0.0f, 1.0f)              // This defines the interval over which the brightness function is defined; [0,1].
  , m_signal_history(2)                   // Only need 2 samples -- current and previous.
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

  m_state_machine.Run(StateMachineEvent::FRAME);
}

void SystemWipeRecognizer::PrintDevInfo (std::ostream &out) const {
  auto percent = [](float b) -> int { return int(100.0f*b); };

  static const size_t PRINT_SAMPLE_COUNT = 22;
#if LEAP_INTERNAL_MEASURE_MAX_BRIGHTNESS
  // Print measured max brightness.
  for (Internal::Linterp<float> t(0.0, 1.0f, PRINT_SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
    float b = MeasuredMaxBrightness(t);
    out << std::setw(3) << percent(b) << ',';
  }
  out << "  ";
#else
  // Print measured brightness.
  for (Internal::Linterp<float> t(0.0, 1.0f, PRINT_SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
    float b = Brightness(t);
    out << std::setw(3) << percent(b) << ',';
  }
  out << "  ";
#endif
  size_t up_edge(std::round((PRINT_SAMPLE_COUNT-1)*CurrentSignal().UpEdge()));
  size_t down_edge(std::round((PRINT_SAMPLE_COUNT-1)*CurrentSignal().DownEdge()));
  size_t centroid(std::round((PRINT_SAMPLE_COUNT-1)*CurrentSignal().Centroid()));
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
    out << (t.Index() == up_edge  ? '[' : underlying_char);
    out << (t.Index() == centroid  ? 'X' : underlying_char);
    out << (t.Index() == down_edge ? ']' : underlying_char);
  }
  out << "  mass: " << std::setw(10) << CurrentSignal().Mass() << ", centroid = " << std::setw(10) << CurrentSignal().Centroid();
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
  }

#if LEAP_INTERNAL_MEASURE_MAX_BRIGHTNESS
  if (m_measured_max_brightness.size() != end_y - begin_y) {
    m_measured_max_brightness.reserve(end_y - begin_y);
    m_measured_max_brightness.clear();
    m_measured_max_brightness.resize(end_y - begin_y, 0.0f); // Reset all to zero.
  }
#endif

  static const bool USE_MAX = false;

  // compute max of row samples
  size_t h_low = (width-1)*0.5f*(1.0f-PROPORTION_OF_IMAGE_WIDTH_TO_USE);
  size_t h_high = (width-1)*0.5f*(1.0f+PROPORTION_OF_IMAGE_WIDTH_TO_USE);
  for (size_t y = begin_y; y < end_y; ++y) {
    uint8_t row_max = 0;
    uint8_t row_min = std::numeric_limits<uint8_t>::max();
    for (Internal::Linterp<size_t> x(h_low, h_high, HORIZONTAL_SAMPLE_COUNT); x.IsNotAtEnd(); ++x) {
      size_t data_index = y*width + x;
      uint8_t brightness = std::max(data0[data_index],data1[data_index]);
      if (brightness > row_max) {
        row_max = brightness;
      }
      if (brightness < row_min) {
        row_min = brightness;
      }
    }
    float brightness = USE_MAX ? row_max/255.0f : row_min/255.0f;
    m_brightness[y - begin_y] = brightness; // Divide by 255.0f to normalize the range [0, 255] to [0.0f, 1.0f].
#if LEAP_INTERNAL_MEASURE_MAX_BRIGHTNESS
    if (brightness > m_measured_max_brightness[y - begin_y]) {
      m_measured_max_brightness[y - begin_y] = brightness;
    }
#endif
  }
}

float SystemWipeRecognizer::ModeledMaxBrightness (float t) {
  // Use a 6th order polynomial to approximate the maximum brightness curve for the
  // computed 1D brightness image.  This is used to normalize the brightness values
  // thereby accounting for the non-uniform LED illumination.  NOTE: This approximates
  // a function that depends heavily on the particular algorithm used to determine
  // the brightness values in m_brightness, so if ComputeBrightness is altered, this
  // function must be updated for the overall algorithm to remain effective.
  // TODO: Maybe make a calibration tool in C++ so that these values could be
  // automatically determined again (this was determined in Sage math system).
  assert(t >= 0.0f && t <= 1.0f);
  static const size_t POLYNOMIAL_ORDER = 6;
  // // This corresponds to the sample data: {39, 44, 50, 59, 69, 80, 88, 93, 97, 97, 97, 97, 97, 97, 93, 88, 80, 69, 59, 50, 44, 39}.
  // static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = { 0.398743, -0.538661, 24.7952, -99.2513, 176.471, -152.215, 50.7382 };
  // // This corresponds to the sample data: {39, 44, 50, 59, 69, 80, 88, 93, 96, 96, 96, 96, 96, 96, 93, 88, 80, 69, 59, 50, 44, 39}.
  // static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = { 0.399369, -0.63521, 26.2851, -106.784, 192.103, -166.453, 55.4844 };
  // // This corresponds to the sample data: {8, 10, 11, 12, 13, 15, 17, 18, 19, 22, 24, 24, 22, 19, 18, 17, 15, 13, 12, 11, 10, 8}.
  // static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = { 0.078959, 0.719451, -6.53487, 32.9726, -69.8408, 64.0254, -21.3418 };
  // // This corresponds to the sample data: {0.12, 0.14, 0.165, 0.2, 0.23, 0.27, 0.29, 0.315, 0.33, 0.36, 0.36, 0.36, 0.36, 0.33, 0.315, 0.29, 0.27, 0.23, 0.2, 0.165, 0.14, 0.12}.
  // static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = { 0.119576, 0.339534, 2.14283, -4.92465, 2.36211, 0.120258, -0.040086 };
  // This corresponds to the sample data: {0.095, 0.105, 0.115, 0.135, 0.14, 0.155, 0.175, 0.185, 0.195, 0.205, 0.21, 0.21, 0.205, 0.195, 0.185, 0.175, 0.155, 0.14, 0.135, 0.115, 0.105, 0.095}.
  static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = { 0.0945103, 0.256714, -0.601845, 5.32955, -14.263, 13.9179, -4.63929 };
  float power_of_t = 1.0f;
  float retval = 0.0f;
  for (size_t i = 0; i < POLYNOMIAL_ORDER+1; ++i) {
    retval += POLYNOMIAL_COEFFICIENTS[i]*power_of_t;
    power_of_t *= t;
  }
  return retval;
}

float SystemWipeRecognizer::NormalizedBrightness (float t) const {
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
      bool detected_sufficient_mass = CurrentSignal().Mass() >= BEGINNING_MASS_ACTIVATION_THRESHOLD;
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
      float complete_tracking_value = begin_tracking_value + PORTION_OF_REMAINDER_UNTIL_COMPLETE*(1.0f - begin_tracking_value); // 70% of the way to the other edge.
      ProgressTransform = [begin_tracking_value, complete_tracking_value](float tracking_value) {
        // The range [begin_tracking_value, complete_tracking_value] shall map onto [0, 1].
        float progress = (tracking_value - begin_tracking_value) / (complete_tracking_value - begin_tracking_value);
        // Clamp to the range [0, 1].
        return std::min(std::max(0.0f, progress), 1.0f);
      };
      *m_system_wipe = SystemWipe{SystemWipe::Status::BEGIN, m_wipe_direction, 0.0f};
      return;
    }
    case StateMachineEvent::FRAME: {
      // There are three ways for the recognition to end:
      // 1. The mass dropping low enough (end without "gesture complete").
      // 2. The wipe gesture proceeding far enough across the screen to register (end with "gesture complete").
      // 3. The wipe regressing back across the screen enough to abort the gesture (end without "gesture complete").
      if (CurrentSignal().Mass() < 0.1f) { // Case 1.
        *m_system_wipe = SystemWipe{SystemWipe::Status::ABORT, m_wipe_direction, 0.0f};
        SET_TRANSITION_REQUEST_AND_RETURN(WaitingForAnyMassSignal);
      }
      auto progress = ProgressTransform(CurrentSignal().TrackingValue(m_wipe_direction));
      if (progress == 1.0f) { // Case 2.
        *m_system_wipe = SystemWipe{SystemWipe::Status::COMPLETE, m_wipe_direction, progress};
        SET_TRANSITION_REQUEST_AND_RETURN(Timeout);
      } else {
        *m_system_wipe = SystemWipe{SystemWipe::Status::UPDATE, m_wipe_direction, progress};
      } // TODO: Case 3.
      return;
    }
    default: return;
  }
}

void SystemWipeRecognizer::Timeout (StateMachineEvent event) {
  PRINT_STATEMACHINE_INFO(Timeout,event);
  switch (event) {
    case StateMachineEvent::ENTER:
      m_timeout_end_time = m_current_time + TIMEOUT_DURATION;
      // Intentionally fall through.
    case StateMachineEvent::FRAME:
      if (m_current_time >= m_timeout_end_time) {
        SET_TRANSITION_REQUEST_AND_RETURN(WaitingForAnyMassSignal);
      }
    default: return;
  }
}
