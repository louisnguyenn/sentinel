#include <iostream>
#include "include/conveyor_line.hpp"

int main() {
    sentinel::ConveyorLine line;
    line.setConveyorRunning(true);
    line.spawnPart();

    const double dt = 0.01; // 10ms tick, matching the real scan cycle later
    for (int i = 0; i < 300; ++i) {   // simulate 3 seconds
        line.update(dt);
        if (line.photoeyeBlocked()) {
            std::cout << "tick " << i << ": photoeye BLOCKED, encoder="
                      << line.encoderCount() << "\n";
        }
    }
    return 0;
}
