#include <cassert>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include "Leap.h"
#include "SystemWipeRecognizer.h"

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

void SampleListener::onFrame(const Controller& controller) {
    // std::cerr << "Frame available" << std::endl;
    SystemWipe system_wipe;
    m_recog.AutoFilter(controller.frame(), system_wipe);
    if (system_wipe.isWiping) {
        std::cerr << "system wipe occurred.  direction: " << FORMAT_VALUE(AsString(system_wipe.direction)) << '\n';
    }
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
