/// phase 1 test harness
// #include <iostream>

// #include "conveyor_line.hpp"

// int main()
// {
//     sentinel::ConveyorLine line;
//     line.setConveyorRunning(true);
//     line.spawnPart();

//     const double dt = 0.01; // 10ms tick, matching the real scan cycle later
//     for (int i = 0; i < 300; ++i)
//     { // simulate 3 seconds
//         line.update(dt);

//         // if (i < 50)
//         // {
//         //     std::cout << "tick " << i << ": motor speed=" << line.motorSpeedMmS() << " mm/s\n";
//         // }

//         if (line.photoeyeBlocked())
//         {
//             std::cout << "tick " << i << ": photoeye BLOCKED, encoder=" << line.encoderCount()
//                       << "\n";
//         }
//     }
//     return 0;
// }

/// phase 2 test harness
#include <iostream>
#include "conveyor_line.hpp"
#include "controller.hpp"

int main() {
    sentinel::ConveyorLine line;
    sentinel::Controller controller(line);

    line.setConveyorRunning(true);
    line.spawnPart();

    const double dt = 0.01;
    uint16_t fake_heartbeat = 0;

    for (int i = 0; i < 400; ++i) {
        line.update(dt);
        controller.tick(dt);

        // Simulate the vision process: whenever the controller starts
        // waiting for a result, respond after a short fake "processing delay."
        if (controller.state() == sentinel::CycleState::AWAIT_RESULT) {
            fake_heartbeat++;
            controller.feedVisionHeartbeat(fake_heartbeat);
            if (i % 5 == 0) {   // pretend vision takes ~5 ticks to respond
                controller.submitInspectionResult(false); // pretend "OK"
            }
        }

        std::cout << "tick " << i << ": state=" << static_cast<int>(controller.state())
                   << " cycles=" << controller.stats().cycle_count
                   << " rejects=" << controller.stats().reject_count << "\n";
    }
    return 0;
}
