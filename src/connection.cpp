#include<stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <array>

#include "connection.hpp"

SWS::Connection::Connection(const int socket_fd) {
    if (socket_fd < 0) {
        throw std::runtime_error("The file descriptor of the connecting socket is invalid! Establishing the connection has failed!");
    }

    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    
    this->client_fd = ::accept(socket_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);

    if (client_fd < 0) {
        throw std::runtime_error("Accepting a new connection failed!");
    }
}

SWS::Connection::~Connection() {
    this->close();
}

SWS::Connection::Connection(Connection&& other) noexcept : client_fd(other.client_fd),buffer_in(std::move(other.buffer_in)), buffer_out(std::move(other.buffer_out)) {
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
    while(!buffer_out.empty()) {
        ssize_t sent = ::send(this->client_fd,
            this->buffer_out.data(),
            this->buffer_out.size(),
            MSG_NOSIGNAL
        );

        if (sent < 0) {
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                return SWS::ConnectionStatus::ERROR;  // something else went wrong! This is not good!
            } else {
                return SWS::ConnectionStatus::WANT_WRITE; // send() buffer is most likey full, we need to send again once its free again!
            }
        }

        size_t actually_sent = static_cast<size_t>(sent);
        this->buffer_out.erase(0, actually_sent); // get rid of already sent data!
    }
    
    return SWS::ConnectionStatus::COMPLETE; // if the send buffer is empty, we can happily exit!
}

SWS::ConnectionStatus SWS::Connection::receive() {
    std::array<char, 4096> buffer;

    while(true) {
        ssize_t bytes_received = ::recv(this->client_fd, buffer.data(), buffer.size(), 0);

        if (bytes_received < 0) {
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                return SWS::ConnectionStatus::ERROR;
            } else {
                return SWS::ConnectionStatus::OPEN; // if the recv() buffer was cleared properly or still has content left.
            }
        }

        if (bytes_received == 0) {
            return SWS::ConnectionStatus::CLOSED; // recv() only returns 0, when the connection has been closed!
        }

        this->buffer_in.append(buffer.data(), bytes_received);

        if (this->buffer_in.size() > MAXIMUM_BUFFER_SIZE) { // value is pickd arbitrarily. Can be changed if needed!
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
        this->client_fd = -1;
    }
}

size_t SWS::Connection::find_request_ending() const {
    return this->buffer_in.find("\r\n\r\n");
}
