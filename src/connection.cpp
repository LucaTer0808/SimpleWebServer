#include<stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <array>
#include <fcntl.h>

#include "connection.hpp"
#include "common/log.hpp"

SWS::Connection::Connection(const int client_fd) : client_fd(client_fd) {
    if (client_fd < 0) {
        SWS::log(SWS::LogLevel::ERROR, "Invalid client file descriptor! It can't be negative. FD: " + std::to_string(client_fd));
        throw std::invalid_argument("Negative file descriptor passed to the connection constructor! It can not represent a valid client socket!");
        this->close();
    }

    int flags = fcntl(client_fd, F_GETFL, 0);

    if (flags == -1) {
        SWS::log_errno("Could not get flags for FD: " + std::to_string(client_fd));
        throw std::runtime_error("fcntl F_GETFL failed");
        this->close();
    }

    if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        SWS::log_errno("Could not set O_NONBLOCK for FD: " + std::to_string(client_fd));
        throw std::runtime_error("fcntl F_SETFL failed");
        this->close();
    }
    
    SWS::log(SWS::LogLevel::INFO, "Connection to client socket established and set to non-blocking with FD: " + std::to_string(this->client_fd));
}

SWS::Connection::~Connection() {
    this->close();
}

SWS::Connection::Connection(Connection&& other) noexcept : client_fd(other.client_fd), buffer_in(std::move(other.buffer_in)), buffer_out(std::move(other.buffer_out)) {
    other.client_fd = -1;
}

SWS::Connection& SWS::Connection::operator=(Connection&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    this->close();

    this->client_fd = other.client_fd;
    other.client_fd = -1;
    this->buffer_in = std::move(other.buffer_in);
    this->buffer_out = std::move(other.buffer_out);

    return *this;
}

SWS::ConnectionStatus SWS::Connection::send(const std::string& data) {
    this->buffer_out.append(data);
    return this->push_data();
}

SWS::ConnectionStatus SWS::Connection::push_data() {
    while(this->bytes_sent_offset < this->buffer_out.size()) {
        ssize_t sent = ::send(this->client_fd,
            this->buffer_out.data() + this->bytes_sent_offset,
            this->buffer_out.size() - this->bytes_sent_offset,
            MSG_NOSIGNAL
        );

        if (sent < 0) {
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                SWS::log_errno("Failed to send data to the client with FD: " + std::to_string(this->client_fd));
                return SWS::ConnectionStatus::ERROR;  // something else went wrong! This is not good!
            } else {
                return SWS::ConnectionStatus::WANT_WRITE; // send() buffer is most likey full, we need to send again once its free again!
            }
        }

        if (sent == 0) {
            SWS::log(SWS::LogLevel::INFO, "No active connection to client with FD: " + std::to_string(this->client_fd));
            return SWS::ConnectionStatus::CLOSED;
        }

        this->bytes_sent_offset += static_cast<size_t>(sent);
    }

    this->buffer_out.clear();
    this->bytes_sent_offset = 0;
    return SWS::ConnectionStatus::COMPLETE; // if the send buffer is empty, we can happily exit!
}

SWS::ConnectionStatus SWS::Connection::receive() {
    std::array<char, 4096> buffer;

    while(true) {
        ssize_t bytes_received = ::recv(this->client_fd, buffer.data(), buffer.size(), 0);

        if (bytes_received < 0) {
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                SWS::log_errno("Failed to receive data from client with FD: " + std::to_string(this->client_fd));
                return SWS::ConnectionStatus::ERROR;
            } else {
                return SWS::ConnectionStatus::OPEN; // if the recv() buffer was cleared properly or still has content left.
            }
        }

        if (bytes_received == 0) {
            SWS::log(SWS::LogLevel::INFO, "No active connection to client with FD: " + std::to_string(this->client_fd));
            return SWS::ConnectionStatus::CLOSED; // recv() only returns 0, when the connection has been closed!
        }

        this->buffer_in.append(buffer.data(), bytes_received);

        if (this->buffer_in.size() > MAXIMUM_BUFFER_SIZE) { // value is pickd arbitrarily. Can be changed if needed!
            SWS::log(SWS::LogLevel::WARNING, "Request size exceeded the healthy limit of " + std::to_string(MAXIMUM_BUFFER_SIZE / 1024) + " kb on client socket with FD: " + std::to_string(this->client_fd));  
            return SWS::ConnectionStatus::PAYLOAD_TOO_LARGE;
        }
    }
}

std::string SWS::Connection::get_latest_request() { 
    size_t pos = this->find_request_ending();

    if (pos == std::string::npos) {
        return "";
    }

    size_t total_len = pos + 4; // \r and \n are accounted for as one byte!

    std::string request = this->buffer_in.substr(0, total_len);
    this->buffer_in.erase(0, total_len);
    return request;
}

void SWS::Connection::close() {
    if (this->client_fd >= 0) {
        ::close(this->client_fd);
        SWS::log(SWS::LogLevel::INFO, "Client socket closed with FD: " + std::to_string(this->client_fd));
        this->client_fd = -1;
    }
}

size_t SWS::Connection::find_request_ending() const {
    return this->buffer_in.find("\r\n\r\n");
}
