#ifndef SENTINEL_MODBUS_SERVER_HPP
#define SENTINEL_MODBUS_SERVER_HPP

#include <modbus/modbus.h>
#include "modbus_registers.hpp"

namespace sentinel
{

/// Thin wrapper around libmodbus: owns the register array and the
/// TCP listening socket, and knows how to service one incoming
/// request without blocking forever if nothing has arrived.
class ModbusServer
{
  public:
    ModbusServer(const char *ip, int port);
    ~ModbusServer();

    /// Non-blocking: services at most one pending request, if any.
    /// Safe to call once per scan cycle.
    void poll();

    /// Direct access to the shared register array, so Controller's
    /// readInputRegisters/writeOutputRegisters can operate on it.
    uint16_t *registers();

  private:
    modbus_t *m_ctx = nullptr;  // Connection settings (IP, port)
    modbus_mapping_t *m_mapping = nullptr;  // Array of Registers
    int m_server_socket = -1;
};

} // namespace sentinel

#endif