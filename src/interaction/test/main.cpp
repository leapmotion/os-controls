#include <cassert>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include "Leap.h"
#include "interaction/SystemWipeRecognizer.h"

using namespace Leap;

#define FORMAT_VALUE(x) #x << " = " << (x)

class SampleListener : public Listener {
public:

    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onFrame(const Controller&);

private:

    SystemWipeRecognizer m_recog;
};


void SampleListener::onConnect(const Controller& controller) {
    std::cerr << "Connected" << std::endl;
}

void SampleListener::onDisconnect(const Controller& controller) {
    std::cerr << "Disconnected" << std::endl;
}

std::string AsString (SystemWipe::Direction d) {
    return d == SystemWipe::Direction::UP ? "UP" : "DOWN";
}

std::string AsString (SystemWipe::Status s) {
    switch (s) { 
        case SystemWipe::Status::NOT_ACTIVE: return "NOT_ACTIVE";
        case SystemWipe::Status::BEGIN:      return "BEGIN";
        case SystemWipe::Status::UPDATE:     return "UPDATE";
        case SystemWipe::Status::COMPLETE:   return "COMPLETE";
        case SystemWipe::Status::ABORT:      return "ABORT";
    }
}

void SampleListener::onFrame(const Controller& controller) {
    // std::cerr << "Frame available" << std::endl;
    SystemWipe system_wipe;
    m_recog.AutoFilter(controller.frame(), system_wipe);
    if (system_wipe.status != SystemWipe::Status::NOT_ACTIVE) {
        std::cerr << " -- system wipe reporting: " << std::setw(4) << AsString(system_wipe.direction) << ", " << std::setw(10) << AsString(system_wipe.status) << ", " << std::setw(10) << system_wipe.progress;
    }
    std::cerr << '\n';
}

int main(int argc, char** argv) {
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
