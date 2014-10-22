#include <cassert>
#include <deque>
#include <Eigen/Dense>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include "Leap.h"

using namespace Leap;

#define FORMAT_VALUE(x) #x << " = " << (x)

template <typename T_, typename IndexType_ = size_t>
class Linterp {
public:

    Linterp (T_ start, T_ end, IndexType_ count)
        :
        m_start(start),
        m_end(end),
        m_count_minus_one(count-1),
        m(0)
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

template <typename T_>
class History {
public:

    History (size_t max_frame_count)
        :
        m_max_frame_count(max_frame_count)
    {
        if (m_max_frame_count == 0) {
            throw std::invalid_argument("max_frame_count must be positive.");
        }
    }

    bool HasEnoughHistoryFor (size_t frame_index) const {
        return m_data.size() >= frame_index;
    }
    const T_ &SampleHistoryAt (size_t frame_index) const {
        return m_data.at(frame_index);
    }

    bool HasEnoughHistoryFor (T_ seconds_of_history, T_ samples_per_second) const {
        size_t max_sample_index = static_cast<size_t>(seconds_of_history*samples_per_second);
        return m_data.size() > max_sample_index;
    }
    template <typename IndexType_>
    void SampleHistory (Linterp<T_,IndexType_> &&time, T_ samples_per_second, std::vector<T_> &out) {
        if (time.LowerBound() < T_(0)) {
            throw std::invalid_argument("time interpolator must have a range in [0,P] for some positive value P.");
        }
        if (!HasEnoughHistoryFor(time.UpperBound(), samples_per_second)) {
            throw std::domain_error("There is not enough history for the requested time range.");
        }
        out.clear();
        for ( ; time.IsNotAtEnd(); ++time) {
            // TODO: linear interpolation
            size_t sample_index = static_cast<size_t>(time*samples_per_second);
            assert(sample_index < m_data.size());
            out.push_back(m_data[sample_index]);
        }
    }

    void RecordFrame (const T_ &value) {
        assert(m_data.size() <= m_max_frame_count);
        // Never exceed m_max_frame_count.
        if (m_data.size() == m_max_frame_count) {
            m_data.pop_back();
        }
        m_data.push_front(value);
    }

private:

    // This defines the maximum number of samples that can be stored.
    size_t m_max_frame_count;
    // Newer samples go on the front of the deques, older samples come off the back.
    std::deque<T_> m_data;
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

// The quadratic is defined by q(t) = p_0 + p_1*t + p_2*t^2.
template <typename T_>
void QuadraticFit (const std::vector<T_> &time, const std::vector<T_> &function, T_ &p_0, T_ &p_1, T_ &p_2, T_ &squared_error) {
    assert(time.size() == function.size() && "There must be the same number of times and function values.");
    Eigen::Matrix<T_,3,3> A;
    Eigen::Matrix<T_,3,1> B;
    T_ C = T_(0);
    A.setZero();
    B.setZero();
    for (size_t i = 0; i < time.size(); ++i) {
        auto t = time[i];
        auto f = function[i];
        Eigen::Matrix<T_,3,1> X(T_(1),t,t*t);
        A += X*X.transpose();
        B -= f*X;
        C += f*f;
    }
    Eigen::Matrix<T_,3,1> P;
    // Least squares solution, in case A is singular.
    P = A.jacobiSvd(Eigen::ComputeFullU|Eigen::ComputeFullV).solve(B);
    p_0 = P(0);
    p_1 = P(1);
    p_2 = P(2);
    squared_error = ((P.transpose()*A*P - T_(2)*B.transpose()*P).eval())(0,0) + C;
    squared_error /= time.size();
}

template <typename T_>
void SimpleFit (const std::vector<T_> &function, T_ &amplitude, T_ &error) {
    T_ A(0), B(0), C(0);
    Linterp<T_> t(T_(0), T_(1), function.size());
    for (auto f : function) {
        T_ q(t*t - t);
        A += T_(16)*q*q;
        B -= T_(4)*f*q;
        C += f*f;
        ++t;
    }
    assert(!t.IsNotAtEnd());
    if (std::abs(A) < std::numeric_limits<T_>::epsilon()) {
        // TODO: error handling
        error = T_(10000); // TEMP HACK
    } else {
        amplitude = B / A;
        error = std::sqrt(std::abs(A*amplitude*amplitude - T_(2)*B*amplitude + C) / function.size());
    }
}

// Fit a symmetric triangular function (piecewise linear over [0,1] with its vertex at 1/2)
// to the given sampled function values, assuming that the function is sampled uniformly
// over [0,1].
template <typename T_>
void TriangleFit (const std::vector<T_> &function, T_ &amplitude, T_ &error) {
    T_ A(0), B(0), C(0);
    static const T_ ONE_HALF = T_(1) / T_(2);
    Linterp<T_> t(T_(0), T_(1), function.size());
    for (auto f : function) {
        T_ one_minus_t(T_(1) - t);
        if (t < ONE_HALF) {
            A += t*t;
            B += f*t;
        } else {
            A += one_minus_t*one_minus_t;
            B += f*one_minus_t;
        }
        C += f*f;
        ++t;
    }
    A *= T_(4);
    B *= T_(2);
    assert(!t.IsNotAtEnd());
    if (std::abs(A) < std::numeric_limits<T_>::epsilon()) {
        // TODO: error handling
        error = T_(10000); // TEMP HACK
    } else {
        amplitude = B / A;
        error = std::sqrt(std::abs(A*amplitude*amplitude - T_(2)*B*amplitude + C) / function.size());
    }
}

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

class SampleListener : public Listener {
public:
    SampleListener ()
        : m_state_machine(*this, &SampleListener::WaitingForMassSignal)
        , m_current_time(0.0)
        , m_centroid_history(1000)
        , m_mass_history(1000)
    {
        for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
            m_weighted_average[i] = 0.0f;
            m_weight[i] = 0.0f;
        }
    }

    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onFrame(const Controller&);

private:

    enum class StateMachineEvent { ENTER, EXIT, FRAME };

    void WaitingForMassSignal (StateMachineEvent);
    void WaitingForHigherMassSignal (StateMachineEvent);
    void WaitingForCentroidDelta (StateMachineEvent);
    void Timeout (StateMachineEvent);

    StateMachine<SampleListener,StateMachineEvent,StateMachineEvent::ENTER,StateMachineEvent::EXIT> m_state_machine;

    // std::vector<std::vector<float>> m_max_brightness_mipmap;
    double m_current_time;
    double m_centroid_signal_start_time;
    double m_timeout_end_time;
    float m_centroid_start_value;
    float m_last_good_centroid_value;
    History<float> m_centroid_history;
    History<float> m_mass_history;
    static const size_t SAMPLE_COUNT = 10;
    float m_weighted_average[SAMPLE_COUNT];
    float m_weight[SAMPLE_COUNT];
};

#define SET_TRANSITION_REQUEST(x) m_state_machine.SetTransitionRequest(&SampleListener::x);
#define PRINT_STATEMACHINE_INFO(state_machine_name,event) \
    do { \
        if (event == StateMachineEvent::ENTER) { \
            std::cerr << '\n' << #state_machine_name << " - ENTER\n"; \
        } else if (event == StateMachineEvent::EXIT) { \
            std::cerr << #state_machine_name << " - EXIT\n\n"; \
        } \
    } while (false)

void SampleListener::WaitingForMassSignal (StateMachineEvent event) {
    PRINT_STATEMACHINE_INFO(WaitingForMassSignal,event);
    switch (event) {
        case StateMachineEvent::FRAME:
            if (m_mass_history.HasEnoughHistoryFor(0) && m_mass_history.SampleHistoryAt(0) >= 0.1f) {
                SET_TRANSITION_REQUEST(WaitingForHigherMassSignal);
                return;
            }
        default: return;
    }
}

void SampleListener::WaitingForHigherMassSignal (StateMachineEvent event) {
    PRINT_STATEMACHINE_INFO(WaitingForHigherMassSignal,event);
    switch (event) {
        case StateMachineEvent::ENTER:
            m_centroid_signal_start_time = m_current_time;
            assert(m_centroid_history.HasEnoughHistoryFor(0));
            m_centroid_start_value = m_centroid_history.SampleHistoryAt(0);
            m_last_good_centroid_value = m_centroid_start_value;
            std::cerr << FORMAT_VALUE(m_centroid_start_value) << "  \n";
            return;
        case StateMachineEvent::FRAME:
            if (m_mass_history.HasEnoughHistoryFor(0)) {
                float current_mass = m_mass_history.SampleHistoryAt(0);
                if (current_mass >= 0.3f) {
                    SET_TRANSITION_REQUEST(WaitingForCentroidDelta);
                    return;
                }
                if (current_mass < 0.1f) {
                    SET_TRANSITION_REQUEST(WaitingForMassSignal);
                    return;
                }
            }
            return;
        default: return;
    }
}

void SampleListener::WaitingForCentroidDelta (StateMachineEvent event) {
    PRINT_STATEMACHINE_INFO(WaitingForCentroidDelta,event);
    switch (event) {
        case StateMachineEvent::FRAME: {
            if (m_mass_history.HasEnoughHistoryFor(0) && m_mass_history.SampleHistoryAt(0) < 0.1f) {
                float delta = m_last_good_centroid_value - m_centroid_start_value;
                std::cerr << FORMAT_VALUE(m_last_good_centroid_value) << ", " << FORMAT_VALUE(m_centroid_start_value) << ", " << FORMAT_VALUE(delta) << '\n';
                if (std::abs(delta) > 0.4f) {
                    std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! centroid delta recognized -- " << (delta > 0.0f ? "DOWN" : "UP") << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                    m_timeout_end_time = m_current_time + 0.3;
                    SET_TRANSITION_REQUEST(Timeout);
                    return;
                }
                SET_TRANSITION_REQUEST(WaitingForMassSignal);
                return;
            } else {
                assert(m_centroid_history.HasEnoughHistoryFor(0));
                m_last_good_centroid_value = m_centroid_history.SampleHistoryAt(0);
            }
            return;
        }
        default: return;
    }
}

void SampleListener::Timeout (StateMachineEvent event) {
    PRINT_STATEMACHINE_INFO(Timeout,event);
    switch (event) {
        case StateMachineEvent::FRAME:
            if (m_current_time >= m_timeout_end_time) {
                SET_TRANSITION_REQUEST(WaitingForMassSignal);
                return;
            }
        default: return;
    }
}

void SampleListener::onConnect(const Controller& controller) {
    std::cerr << "Connected" << std::endl;
    m_state_machine.Initialize();
}

void SampleListener::onDisconnect(const Controller& controller) {
    std::cerr << "Disconnected" << std::endl;
    m_state_machine.Shutdown();
}

void SampleListener::onFrame(const Controller& controller) {
    // std::cerr << "Frame available" << std::endl;

    m_current_time = 1.0e-6 * controller.frame().timestamp();

    Leap::ImageList images = controller.frame().images();
    // std::cerr << "number of images = " << images.count() << '\n';
    if (images.count() < 2) {
        return;
    }

    if (!(images[0].isValid() && images[1].isValid())) {
        return;
    }

    // Sample each of the images along vertical lines
    std::vector<float> brightness;
    {
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
        std::vector<float> row;
        row.reserve(HORIZONTAL_SAMPLE_COUNT);
        for (size_t y = 0; y < height; ++y) {
            row.clear();
            for (Linterp<size_t> x(0, width-1, HORIZONTAL_SAMPLE_COUNT); x.IsNotAtEnd(); ++x) {
                size_t data_index = y*width + x;
                row.push_back(std::max(data0[data_index],data1[data_index]) / 255.0f);
            }
            brightness.push_back(Average(row));
        }
    }

    // if (m_max_brightness_mipmap.size() != brightness_mipmap.size()) {
    //     m_max_brightness_mipmap.resize(brightness_mipmap.size());
    // }

    // auto take_max_of = [](const std::vector<float> &v, std::vector<float> &dest) {
    //     assert(v.size() == dest.size());
    //     for (size_t i = 0; i < v.size(); ++i) {
    //         if (dest[i] < v[i]) {
    //             dest[i] = v[i];
    //         }
    //     }
    // };
    // for (size_t i = 0; i < brightness_mipmap.size(); ++i) {
    //     if (m_max_brightness_mipmap[i].size() != brightness_mipmap[i].size()) {
    //         m_max_brightness_mipmap[i].resize(brightness_mipmap[i].size(), 0.0f);
    //     }
    //     take_max_of(brightness_mipmap[i], m_max_brightness_mipmap[i]);
    // }

    // Use a 6th order polynomial to approximate the maximum brightness curve for the
    // center vertical line for the camera.
    // TODO: Maybe make a calibration tool in C++ so that these values could be
    // automatically determined again (this was determined in Sage math system).
    auto max_brightness_approximation = [](float t) {
        assert(t >= 0.0f && t <= 1.0f);
        // t |-->   69.972969779  * t^6 - 204.911368221 * t^5 + 219.060195969 * t^4
        //        - 102.001675189 * t^3 + 16.2145167682 * t^2 + 1.70793619066 * t
        //        + 0.43664087912
        static const size_t POLYNOMIAL_ORDER = 6;
        // static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = {
        //        0.43664087912f, // constant term
        //        1.70793619066f, // linear term
        //       16.2145167682f,  // quadratic term
        //     -102.001675189f,   // etc.
        //      219.060195969f,
        //     -204.911368221f,
        //       69.972969779
        // };
        // static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = { 0.707890459553f, 4.32712858129f, -25.753521887f, 75.4670191074f, -117.540522017f, 93.9155909561f, -30.3436435936f };
        // static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = { 0.410510438512f, 0.827091349285f, 25.0505350679f, -133.066847156f, 269.812409302f, -243.934782779f, 81.311594216f };
        static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = { 0.509999999995f, 2.08500000785f, -2.29200009331f, -5.44499960929f, 17.5499992571f, -17.819999345f, 5.83199978266f };
        float power_of_t = 1.0f;
        float retval = 0.0f;
        for (size_t i = 0; i < POLYNOMIAL_ORDER+1; ++i) {
            retval += POLYNOMIAL_COEFFICIENTS[i]*power_of_t;
            power_of_t *= t;
        }
        return retval;
    };

    auto percent = [](float b) -> int {
        return int(100.0f*b);
    };
    auto snap = [](float b) -> int {
        return (b > 0.8f) ? 1 : 0;
    };

    auto filtered_value = snap;

    // std::vector<float> approximated_max_brightness;
    // for (int i = 0; i < m_max_brightness_mipmap[3].size(); ++i) {
    //     float t = float(i) / (m_max_brightness_mipmap[3].size() - 1);
    //     approximated_max_brightness.push_back(max_brightness_approximation(t));
    // }

    // std::cerr << "    ";
    // for (auto b : m_max_brightness_mipmap[3]) {
    //     std::cerr << std::setw(3) << percent(b) << ',';
    // }
    // std::cerr << '\n';

    // std::cerr << "    ";
    // for (auto b : approximated_max_brightness) {
    //     std::cerr << std::setw(3) << filtered_value(b) << ',';
    // }
    // std::cerr << '\n';

    // std::cerr << "    ";
    // for (auto b : brightness_mipmap[3]) {
    //     std::cerr << std::setw(3) << percent(b) << ',';
    // }
    // std::cerr << '\n';

    Mipmap<float> mipmap(brightness);

    // Number of samples uniformly distributed.  TODO: compute which mipmap level to use based on this.
    float mass = 0.0f;
    float centroid = 0.0f;
    for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
        float t = float(i) / (SAMPLE_COUNT-1);
        float b = mipmap.Sample(3,t);
        if (b > 0.2f) {
            m_weighted_average[i] += b;
            m_weight[i] += 1.0f;
        }
        b /= max_brightness_approximation(t);
        // float s = b; //snap(b);
        float s = snap(b);
        mass += s;
        centroid += s*t;
    }
    if (mass > std::numeric_limits<float>::epsilon()) {
        centroid /= mass; // Divide centroid through by mass before adjusting mass by sample_count.
    }
    mass /= SAMPLE_COUNT;
    // std::cerr << "previous: " << FORMAT_VALUE(m_centroid_history.SampleHistoryAt(0)) << "  ";
    m_centroid_history.RecordFrame(centroid);
    // std::cerr << "next: " << FORMAT_VALUE(m_centroid_history.SampleHistoryAt(0)) << "  ";
    m_mass_history.RecordFrame(mass);

    // for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
    //     float t = float(i) / (SAMPLE_COUNT-1);
    //     float b = mipmap.Sample(3,t) / max_brightness_approximation(t);
    //     std::cerr << std::setw(3) << percent(b);
    // }

    // std::cerr << "    ";
    // for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
    //     float t = float(i) / (SAMPLE_COUNT-1);
    //     float average = 0.0f;
    //     if (m_weight[i] > 0.0f) {
    //         average = m_weighted_average[i] / m_weight[i];
    //     }
    //     std::cerr << std::setw(3) << percent(average) << ',';
    // }

    // for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
    //     float t = float(i) / (SAMPLE_COUNT-1);
    //     float b = mipmap.Sample(3,t) / max_brightness_approximation(t);
    //     std::cerr << std::setw(3) << percent(b) << ',';
    //     // std::cerr << std::setw(1) << snap(b);
    // }

    // for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
    //     float t = float(i) / (SAMPLE_COUNT-1);
    //     float b = mipmap.Sample(3,t) / max_brightness_approximation(t);
    //     // std::cerr << std::setw(3) << filtered_value(b) << ',';
    //     std::cerr << (i == size_t(std::round(centroid*(SAMPLE_COUNT-1))) ? 'X' : '.');
    // }
    // std::cerr << ", " << std::setw(3) << percent(mass) << "  " << std::setw(3) << percent(centroid) << '\n';

    // float instantaneous_frame_rate = controller.frame().currentFramesPerSecond();
    // static const size_t HISTORICAL_SAMPLE_COUNT = 10;
    // static const size_t HISTORY_SCALE_COUNT = 4;
    // float seconds_of_history = 0.5f;
    // float scale_factor = 0.5f;//std::sqrt(0.5f);
    // std::vector<std::vector<float>> mass_histories;
    // mass_histories.resize(HISTORY_SCALE_COUNT);
    // if (m_mass_history.HasEnoughHistoryFor(seconds_of_history, instantaneous_frame_rate)) {
    //     for (size_t scale_index = 0; scale_index < HISTORY_SCALE_COUNT; ++scale_index) {
    //         std::vector<float> &mass_history = mass_histories[scale_index];
    //         m_mass_history.SampleMassHistory(Linterp<float>(0.0f, seconds_of_history, HISTORICAL_SAMPLE_COUNT), instantaneous_frame_rate, mass_history);
    //         seconds_of_history *= scale_factor;
    //         for (auto m : mass_history) {
    //             std::cerr << std::setw(3) << percent(m);
    //         }
    //         std::cerr << ", ";

    //         std::vector<float> time;
    //         for (Linterp<float> t(0.0f, 1.0f, HISTORICAL_SAMPLE_COUNT); t.IsNotAtEnd(); ++t) {
    //             time.push_back(t);
    //         }
    //         // float p_0, p_1, p_2;
    //         float amplitude;
    //         float error;
    //         // QuadraticFit(time, mass_history, p_0, p_1, p_2, squared_error);
    //         // SimpleFit(mass_history, amplitude, error);
    //         TriangleFit(mass_history, amplitude, error);
    //         if (amplitude > 0.0f) {// && error < 0.1f) {
    //             std::cerr << std::setw(3) << percent(amplitude) << " " << std::setw(10) << error << ", ";
    //         } else {
    //             std::cerr << "    " << std::setw(10) << 'x' << ", ";
    //         }
    //     }
    // }



    // if (m_mass_history.HasEnoughHistoryFor()) {
    //     std::cerr << "    ";
    //     for (size_t i = 0; i < HISTORICAL_SAMPLE_COUNT; ++i) {
    //         float t = float(i) / (HISTORICAL_SAMPLE_COUNT-1);
    //         float centroid, mass;
    //         m_mass_history.Sample(t, centroid, mass);
    //         std::cerr << std::setw(3) << percent(mass);
    //     }

    //     // std::cerr << "    ";
    //     // for (size_t i = 0; i < HISTORICAL_SAMPLE_COUNT; ++i) {
    //     //     float t = float(i) / (HISTORICAL_SAMPLE_COUNT-1);
    //     //     float centroid, mass;
    //     //     m_mass_history.Sample(t, centroid, mass);
    //     //     std::cerr << std::setw(3) << percent(centroid);
    //     // }
    // }

    m_state_machine.Run(StateMachineEvent::FRAME);

    // std::cerr << '\n';
}

int main(int argc, char** argv) {
    // std::vector<float> f;
    // float expected_amplitude = 3.14f;
    // for (Linterp<float> t(0.0f, 1.0f, 10); t.IsNotAtEnd(); ++t) {
    //     f.push_back(-4.0*expected_amplitude*(t*t - t));
    // }
    // float computed_amplitude, error;
    // SimpleFit<float>(f, computed_amplitude, error);
    // std::cerr << FORMAT_VALUE(expected_amplitude) << ", " << FORMAT_VALUE(computed_amplitude) << ", " << FORMAT_VALUE(error) << '\n';
    // return 0;

    // std::vector<float> f;
    // float expected_amplitude = 3.14f;
    // auto func = [expected_amplitude](float t) {
    //     return t < 0.5f ? 2.0f*expected_amplitude*t : 2.0f*expected_amplitude*(1.0f-t);
    // };
    // for (Linterp<float> t(0.0f, 1.0f, 10); t.IsNotAtEnd(); ++t) {
    //     f.push_back(func(t));
    // }
    // float computed_amplitude, error;
    // TriangleFit<float>(f, computed_amplitude, error);
    // std::cerr << FORMAT_VALUE(expected_amplitude) << ", " << FORMAT_VALUE(computed_amplitude) << ", " << FORMAT_VALUE(error) << '\n';
    // return 0;


    SampleListener listener;
    Controller controller;

    // Set the policy flag so that images are available
    controller.setPolicyFlags(static_cast<Controller::PolicyFlag>(Controller::PolicyFlag::POLICY_IMAGES|controller.policyFlags()));

    controller.addListener(listener);

    // Keep this process running until Enter is pressed
    std::cerr << "Press Enter to quit..." << std::endl;
    std::cin.get();

    // Remove the sample listener when done
    controller.removeListener(listener);

    return 0;
}
