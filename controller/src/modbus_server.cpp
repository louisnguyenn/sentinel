#include "modbus_server.hpp"
#include <stdexcept>

sentinel::ModbusServer::ModbusServer(const char* ip, int port)
{
    m_ctx = modbus_new_tcp(ip, port);
    if (!m_ctx)
    {
        throw std::runtime_error("Failed to create Modbus context");
    }

    m_mapping = modbus_mapping_new(0, 0, REG_COUNT, 0);
    if (!m_mapping)
    {
        throw std::runtime_error("Failed to allocate Modbus registers");
    }

    m_server_socket = modbus_tcp_listen(m_ctx, 1);
    if (!m_server_socket)
    {
        throw std::runtime_error("Failed to listen on Modbus TCP");
    }

    // Non-blocking accept: don't freeze the whole controller while waiting for a client to connect
    modbus_set_socket(m_ctx, m_server_socket);
}

sentinel::ModbusServer::~ModbusServer()
{
    /// Free memory
    if (m_mapping)
    {
        modbus_mapping_free(m_mapping);
    }

    if (m_ctx)
    {
        modbus_close(m_ctx);
        modbus_free(m_ctx);
    }
}

void sentinel::ModbusServer::poll()
{
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

    /// Checks if there is a request
    /// Returns 0 if not request
    int rc = modbus_receive(m_ctx, query);

    // rc == 0: no request pending this tick - not an error
    // rc > 0: an error occured - log error
    if (rc > 0)
    {
        modbus_reply(m_ctx, query, rc, m_mapping); // Answers whatever request came in
    }
}

uint16_t* sentinel::ModbusServer::registers()
{
}
