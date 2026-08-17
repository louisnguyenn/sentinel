/// phase 3 test harness
#include "controller.hpp"
#include "conveyor_line.hpp"
#include "modbus_server.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    sentinel::ModbusServer modbus("0.0.0.0", 5020);

    sentinel::ConveyorLine line;
    sentinel::Controller controller(line);

    line.setConveyorRunning(true);
    line.spawnPart();

    const double dt = 0.01;
    uint16_t fake_heartbeat = 0;
    bool second_part_spawned = false;

    const double spawn_interval_s = 3.0; // try to introduce a new part every 3 simulated seconds
    double time_since_last_spawn = 0.0;

    sentinel::CycleState last_printed_state = controller.state();
    int tick_count = 0;

    for (;;)
    {
        auto scan_start = std::chrono::steady_clock::now();

        modbus.poll();
        controller.readInputRegisters(modbus.registers());

        time_since_last_spawn += dt;
        if (time_since_last_spawn >= spawn_interval_s &&
            controller.state() == sentinel::CycleState::IDLE)
        {
            line.spawnPart();
            time_since_last_spawn = 0.0;
            std::cout << "tick " << tick_count << ": spawned a new part\n";
        }

        line.update(dt);
        controller.tick(dt);

        controller.writeOutputRegisters(modbus.registers());

        if (controller.state() != last_printed_state)
        {
            std::cout << "tick " << tick_count << ": state changed to "
                      << static_cast<int>(controller.state());
            if (controller.state() == sentinel::CycleState::FAULT)
            {
                std::cout << " (fault code=" << static_cast<int>(controller.activeFault()) << ")";
            }
            std::cout << "\n";
            last_printed_state = controller.state();
        }

        ++tick_count;

        auto elapsed = std::chrono::steady_clock::now() - scan_start;
        auto target = std::chrono::duration<double>(dt);
        if (elapsed < target)
        {
            std::this_thread::sleep_for(target - elapsed);
        }
    }

    return 0;
}
