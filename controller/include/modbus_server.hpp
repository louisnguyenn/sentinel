#ifndef SENTINEL_MODBUS_SERVER_HPP
#define SENTINEL_MODBUS_SERVER_HPP

#include <modbus/modbus.h>
#include <vector>
#include "modbus_registers.hpp"

namespace sentinel
{

/// Modbus TCP server supporting multiple simultaneous clients (vision,
/// HMI, etc.) via select()-based polling instead of a single blocking
/// accept.
class ModbusServer
{
  public:
    ModbusServer(const char *ip, int port);
    ~ModbusServer();

    /// Non-blocking: accepts any new clients, services any clients with
    /// a pending request. Safe to call once per scan cycle.
    void poll();

    uint16_t *registers();

  private:
    modbus_t *m_ctx = nullptr;
    modbus_mapping_t *m_mapping = nullptr;
    int m_listen_socket = -1;

    std::vector<int> m_client_sockets;
};

} // namespace sentinel

#endif
