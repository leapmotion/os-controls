#include <cassert>
#include <iomanip>
#include <iostream>
#include <string.h>
#include "Leap.h"

using namespace Leap;

#define FORMAT_VALUE(x) #x << " = " << (x)



class SampleListener : public Listener {
public:
    virtual void onConnect(const Controller&);
    virtual void onFrame(const Controller&);

private:

    std::vector<std::vector<float>> m_max_brightness_mipmap;
};

void SampleListener::onConnect(const Controller& controller) {
    std::cout << "Connected" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
    // std::cout << "Frame available" << std::endl;

    Leap::ImageList images = controller.frame().images();
    // std::cout << "number of images = " << images.count() << '\n';
    if (images.count() < 2) {
        return;
    }

    Leap::Image image = images[1]; // Use the right image
    // std::cout << "image isValid = " << image.isValid() << '\n';
    if (!image.isValid()) {
        return;
    }

    // Make a mipmap ofr the brightnesses.
    std::vector<std::vector<float>> brightness_mipmap(1);
    brightness_mipmap.reserve(std::ceil(1.0f+std::log2(image.height())));

    std::vector<float> &brightness = brightness_mipmap.back();
    if (true) {
        // use the vertical scanline a fraction of the way across the screen.
        int x = 1*image.width()/2;
        for (int y = 0; y < image.height(); ++y) {
            int data_index = y*image.width() + x;
            brightness.push_back(image.data()[data_index] / 255.0f);
        }
    } else {
        size_t sample_count = 20;
        float vertical_angle_begin = 20.0f;
        float vertical_angle_end = -20.0f;
        for (float vertical_angle = vertical_angle_begin;
             vertical_angle >= vertical_angle_end;
             vertical_angle -= std::abs(vertical_angle_end-vertical_angle_begin) / (sample_count-1))
        {
            float horizontal_slope = 0.0f;
            float vertical_slope = tan(vertical_angle * 180.0f/Leap::PI);
            Leap::Vector pixel = image.warp(Leap::Vector(horizontal_slope, vertical_slope, 0.0f));
            std::cout << FORMAT_VALUE(pixel) << '\n';
            int data_index = std::floor(pixel.y) * image.width() + std::floor(pixel.x);
            brightness.push_back(image.data()[data_index] / 255.0f);
        }
    }

    auto downsample = [](const std::vector<float> &in, std::vector<float> &out) {
        out.clear();
        for (size_t y = 0; y+1 < in.size(); y += 2) {
            out.push_back(0.5f*(in[y]+in[y+1]));
        }
    };

    const std::vector<float> *previous = &brightness_mipmap.back();
    while (previous->size()/2 > 0) {
        brightness_mipmap.emplace_back();
        std::vector<float> &next = brightness_mipmap.back();
        downsample(*previous, next);
        previous = &next;
    }

    if (m_max_brightness_mipmap.size() != brightness_mipmap.size()) {
        m_max_brightness_mipmap.resize(brightness_mipmap.size());
    }

    auto take_max_of = [](const std::vector<float> &v, std::vector<float> &dest) {
        assert(v.size() == dest.size());
        for (size_t i = 0; i < v.size(); ++i) {
            if (dest[i] < v[i]) {
                dest[i] = v[i];
            }
        }
    };
    for (size_t i = 0; i < brightness_mipmap.size(); ++i) {
        if (m_max_brightness_mipmap[i].size() != brightness_mipmap[i].size()) {
            m_max_brightness_mipmap[i].resize(brightness_mipmap[i].size(), 0.0f);
        }
        take_max_of(brightness_mipmap[i], m_max_brightness_mipmap[i]);
    }

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
        static const float POLYNOMIAL_COEFFICIENTS[POLYNOMIAL_ORDER+1] = { 0.410510438512f, 0.827091349285f, 25.0505350679f, -133.066847156f,
269.812409302f, -243.934782779f, 81.311594216f };
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

    // auto filtered_value = percent;
    auto filtered_value = snap;

    // std::vector<float> approximated_max_brightness;
    // for (int i = 0; i < m_max_brightness_mipmap[3].size(); ++i) {
    //     float t = float(i) / (m_max_brightness_mipmap[3].size() - 1);
    //     approximated_max_brightness.push_back(max_brightness_approximation(t));
    // }

    // std::cout << "    ";
    // for (auto b : m_max_brightness_mipmap[3]) {
    //     std::cout << std::setw(3) << percent(b) << ',';
    // }
    // std::cout << '\n';

    // std::cout << "    ";
    // for (auto b : approximated_max_brightness) {
    //     std::cout << std::setw(3) << filtered_value(b) << ',';
    // }
    // std::cout << '\n';

    // std::cout << "    ";
    // for (auto b : brightness_mipmap[3]) {
    //     std::cout << std::setw(3) << percent(b) << ',';
    // }
    // std::cout << '\n';

    float mass = 0.0f;
    float centroid = 0.0f;
    size_t sample_count = brightness_mipmap[3].size();
    for (int i = 0; i < sample_count; ++i) {
        float t = float(i) / (sample_count - 1);
        float b = brightness_mipmap[3][i] / max_brightness_approximation(t);
        float s = snap(b);
        mass += s;
        centroid += s*t;
    }
    centroid /= mass; // Divide centroid through by mass before adjusting mass by sample_count.
    mass /= sample_count;

    for (int i = 0; i < brightness_mipmap[3].size(); ++i) {
        float t = float(i) / (brightness_mipmap[3].size() - 1);
        float b = brightness_mipmap[3][i] / max_brightness_approximation(t);
        // std::cout << std::setw(3) << filtered_value(b) << ',';
        std::cout << std::setw(1) << filtered_value(b);
    }
    // std::cout << '\n';

    std::cout << "    ";
    for (int i = 0; i < brightness_mipmap[3].size(); ++i) {
        float t = float(i) / (brightness_mipmap[3].size() - 1);
        float b = brightness_mipmap[3][i] / max_brightness_approximation(t);
        // std::cout << std::setw(3) << filtered_value(b) << ',';
        std::cout << (i == int(std::round(centroid*sample_count)) ? 'X' : '.');
    }
    std::cout << "    " << std::setw(3) << percent(mass) << '\n';

    // std::cout << '\n';
}

int main(int argc, char** argv) {
    SampleListener listener;
    Controller controller;

    // Set the policy flag so that images are available
    controller.setPolicyFlags(
        static_cast<Controller::PolicyFlag>(Controller::PolicyFlag::POLICY_IMAGES|controller.policyFlags()));

    controller.addListener(listener);

    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    // Remove the sample listener when done
    controller.removeListener(listener);

    return 0;
}
