#ifndef SENTINEL_MODBUS_REGISTERS_HPP
#define SENTINEL_MODBUS_REGISTERS_HPP

namespace sentinel
{

/// Holding register index map. Both the controller and every Python
/// client (vision, HMI) must agree on these indices — this is the
/// entire "protocol" between them.
enum RegisterIndex
{
    REG_PHOTOEYE            = 0,  // controller writes: 1 = part present
    REG_ENCODER_COUNT       = 1,  // controller writes
    REG_TRIGGER_CAPTURE     = 2,  // controller writes: 1 = vision should inspect now
    REG_INSPECTION_RESULT   = 3,  // vision writes: 0 = OK, 1 = defective
    REG_VISION_HEARTBEAT    = 4,  // vision writes: increments every vision cycle
    REG_DIVERTER_CMD        = 5,  // controller writes
    REG_DIVERTER_FEEDBACK   = 6,  // controller writes
    REG_MODE_SELECT         = 7,  // HMI writes: 0=Auto, 1=Manual, 2=Maintenance
    REG_ESTOP               = 8,  // HMI writes: 1 = e-stop active
    REG_RESET_FAULT         = 9,  // HMI writes: momentary 1 = attempt reset
    REG_CYCLE_COUNT         = 10, // controller writes
    REG_REJECT_COUNT        = 11, // controller writes
    REG_FAULT_COUNT         = 12, // controller writes
    REG_MACHINE_STATE       = 13, // controller writes: CycleState as an int
    REG_MANUAL_CONVEYOR_JOG = 14, // HMI writes (Manual mode only)
    REG_MANUAL_DIVERTER_JOG = 15, // HMI writes (Manual mode only)
    REG_RESULT_SEQ = 16,    // vision writes: increments each time it posts a new result

    REG_COUNT               = 17  // total register count — use this to size the array
};

} // namespace sentinel

#endif
