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
//         //     std::cout << "tick " << i << ": motor speed=" << line.motorSpeedMmS() << "
//         mm/s\n";
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
#include "controller.hpp"
#include "conveyor_line.hpp"
#include <iostream>

int main()
{
    sentinel::ConveyorLine line;
    sentinel::Controller controller(line);

    line.setConveyorRunning(true);
    line.spawnPart();

    const double dt = 0.01;
    uint16_t fake_heartbeat = 0;
    bool second_part_spawned = false;

    for (int i = 0; i < 700; ++i)
    { // extended from 400 — see note below
        line.update(dt);
        controller.tick(dt);

        if (controller.state() == sentinel::CycleState::AWAIT_RESULT)
        {
            fake_heartbeat++;
            controller.feedVisionHeartbeat(fake_heartbeat);
            if (i % 5 == 0)
            {
                controller.submitInspectionResult(false);
            }
        }

        // Spawn a second part well after the first one has cleared IDLE.
        if (i == 350 && !second_part_spawned)
        {
            line.spawnPart();
            second_part_spawned = true;
        }

        std::cout << "tick " << i << ": state=" << static_cast<int>(controller.state())
                  << " cycles=" << controller.stats().cycle_count
                  << " rejects=" << controller.stats().reject_count << "\n";
    }

    return 0;
}

/// phase 3 test harness
#include "controller.hpp"
#include "conveyor_line.hpp"
#include "modbus_server.hpp"
#include <iostream>

int main()
{
    sentinel::ModbusServer modbus("0.0.0.0", 502);

    sentinel::ConveyorLine line;
    sentinel::Controller controller(line);

    line.setConveyorRunning(true);
    line.spawnPart();

    const double dt = 0.01;
    uint16_t fake_heartbeat = 0;
    bool second_part_spawned = false;

    for (;;)
    {
        modbus.poll();
        controller.readInputRegisters(modbus.registers());

        line.update(dt);
        controller.tick(dt);

        controller.writeOutputRegisters(modbus.registers());
    }

    return 0;
}
