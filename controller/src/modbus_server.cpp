#include "modbus_server.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <sys/select.h>

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

    m_listen_socket = modbus_tcp_listen(m_ctx, 5); // allow up to 5 connected clients
    if (m_listen_socket == -1)
    {
        throw std::runtime_error("Failed to listen on Modbus TCP");
    }
}

sentinel::ModbusServer::~ModbusServer()
{
    /// Free memory
    for (int socket : m_client_sockets)
    {
        modbus_close(m_ctx);
    }

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

// TODO: refactor poll to take more than one connected client
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
    return m_mapping->tab_registers;
}
