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

void sentinel::ModbusServer::poll()
{
    // Build the set of sockets we want to watch: the listening socket
    // (for new connections) plus every currently connected client.
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(m_listen_socket, &read_fds);
    int max_fd = m_listen_socket;

    for (int sock : m_client_sockets)
    {
        FD_SET(sock, &read_fds);
        max_fd = std::max(max_fd, sock);
    }

    // Don't block at all — return immediately with whatever's ready
    // right now, since poll() is called once per scan cycle.
    timeval timeout{0, 0};
    int ready = select(max_fd + 1, &read_fds, nullptr, nullptr, &timeout);
    if (ready <= 0)
    {
        return; // nothing to do this cycle
    }

    // New client trying to connect?
    if (FD_ISSET(m_listen_socket, &read_fds))
    {
        int new_socket = modbus_tcp_accept(m_ctx, &m_listen_socket);
        if (new_socket != -1)
        {
            m_client_sockets.push_back(new_socket);
            std::cout << "ModbusServer: client connected (fd=" << new_socket
                      << ", total clients=" << m_client_sockets.size() << ")\n";
        }
    }

    // Service any existing client that has a request pending.
    for (auto it = m_client_sockets.begin(); it != m_client_sockets.end();)
    {
        if (FD_ISSET(*it, &read_fds))
        {
            modbus_set_socket(m_ctx, *it);

            uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
            int rc = modbus_receive(m_ctx, query);
            if (rc > 0)
            {
                modbus_reply(m_ctx, query, rc, m_mapping);
                ++it;
            }
            else
            {
                // Client disconnected — stop tracking it.
                modbus_close(m_ctx);
                std::cout << "ModbusServer: client disconnected (fd=" << *it << ")\n";
                it = m_client_sockets.erase(it);
            }
        }
        else
        {
            ++it;
        }
    }
}

uint16_t* sentinel::ModbusServer::registers()
{
    return m_mapping->tab_registers;
}
