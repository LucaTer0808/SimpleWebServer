#include "socket.hpp"
#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>
#include <fcntl.h>
#include <format>
#include <string>

#include "common/log.hpp"

SWS::Socket::Socket(const uint16_t port) {
    this->socket_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (socket_fd == -1) {
        SWS::log_errno("Creating a listening socket failed!");
        throw std::runtime_error("::socketAF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0) failed!");
    }

    int opt = 1;
    int setsockopt_result = setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (setsockopt_result < 0) {
        this->close();
        SWS::log_errno("Setting socket options failed with FD: " + std::to_string(socket_fd));
        throw std::runtime_error("setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) failed!");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    int bind_result = ::bind(socket_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));

    if (bind_result < 0) {
        this->close();
        SWS::log_errno("Binding the socket with FD " + std::to_string(socket_fd) + " to port " + std::to_string(port) + " failed!");
        throw std::runtime_error("::bind(socket_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) failed!");
    }

    int listen_result = listen(this->socket_fd, SOMAXCONN);

    if (listen_result < 0) {
        this->close();
        SWS::log_errno("Making the socket listen has failed with FD: " + std::to_string(socket_fd));
        throw std::runtime_error("listen(this->socket_fd, SOMAXCONN) has failed!");
    }

    SWS::log(SWS::LogLevel::INFO, "Socket with FD: " + std::to_string(socket_fd) + " is now listening on port: " + std::to_string(port));
}

SWS::Socket::~Socket() {
    this->close();
}

SWS::Socket::Socket(Socket&& other) noexcept {
    this->socket_fd = other.socket_fd;
    other.socket_fd = -1;
}

SWS::Socket& SWS::Socket::operator=(Socket&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    this->close();

    this->socket_fd = other.socket_fd;
    other.socket_fd = -1;

    return *this;
}

std::vector<std::unique_ptr<SWS::Connection>> SWS::Socket::accept() {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    std::vector<std::unique_ptr<SWS::Connection>> conns;

    while (true) {
        int client_fd = ::accept(this->socket_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);

        if (client_fd < 0) {
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                SWS::log_errno("An error occured when trying to establish a connection! Continueing with the next connection!");
                continue;
            } else {
                break; // happens when no more connectinos are in the buffer
            }
        }

        try {
            conns.push_back(std::make_unique<SWS::Connection>(client_fd));
        } catch (const std::exception& e) {
            continue;
        }
    }

    return conns;
}

void SWS::Socket::close() {
    if (this->socket_fd >= 0) {
        ::close(this->socket_fd);
        SWS::log(SWS::LogLevel::INFO, "Listening Socket closed with FD: " + std::to_string(this->socket_fd));
        this->socket_fd = -1;
    }
}
