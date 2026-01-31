#include<stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <stdexcept>
#include <array>

#include "connection.hpp"

SWS::Connection::Connection(const int client_fd) : client_fd(client_fd) {
    if (client_fd < 0) {
        throw std::runtime_error("The file descriptor of the client is negative! Establishing the connection has failed!");
    }
}

SWS::Connection::~Connection() {
    this->close();
}

SWS::Connection::Connection(Connection&& other) noexcept {
    this->client_fd = other.client_fd;
    other.client_fd = -1;
}

SWS::Connection& SWS::Connection::operator=(Connection&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    this->close();

    this->client_fd = other.client_fd;
    other.client_fd = -1;

    return *this;
}

void SWS::Connection::send(const std::string& data) {
    size_t total_sent = 0;
    size_t data_length = data.length();

    while (total_sent < data_length) {
        ssize_t sent = ::send(
            this->client_fd,
            data.c_str() + total_sent,
            data_length - total_sent,
            0
        );

        if (sent < 0) {
            throw std::runtime_error("Failed to send data to client! Socket-ID: " + std::to_string(this->client_fd));
        }

        total_sent += static_cast<size_t>(sent);
    }
}

std::string SWS::Connection::receive() {
    std::array<char, 4096> buffer;

    ssize_t bytes_received = ::recv(this->client_fd, buffer.data(), buffer.size(), 0);

    if (bytes_received < 0) {
        throw std::runtime_error("Failed to receive data from the client!");
    }

    if (bytes_received == 0) {
        return "";
    }

    return std::string(buffer.data(), bytes_received);
}

void SWS::Connection::close() {
    if (this->client_fd >= 0) {
        ::close(this->client_fd);
        this->client_fd = -1;
    }
}