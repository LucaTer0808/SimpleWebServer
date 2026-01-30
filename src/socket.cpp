#include "socket.hpp"
#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>

SWS::Socket::Socket(uint16_t port) {
    this->socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    int bind_result = bind(socket_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));

    if (bind_result < 0) {
        ::close(this->socket_fd);
        throw std::runtime_error("Socket binding failed! Most likely, the port " + std::to_string(port) + " is already taken!");
    }

    int listen_result = listen(this->socket_fd, SOMAXCONN);

    if (listen_result < 0) {
        ::close(this->socket_fd);
        throw std::runtime_error("The socket can not listen on port " + std::to_string(port));
    }
}

SWS::Socket::~Socket() {
    for (auto &client_fd : this->clients) {
        this->closeConnection(client_fd);
    }

    ::close(this->socket_fd);
}

int SWS::Socket::acceptConnection() {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = ::accept(this->socket_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);

    if (client_fd < 0) {
        throw std::runtime_error("Accepting a new connection failed!");
    }

    this->clients.push_back(client_fd);
    return client_fd;
}

void SWS::Socket::closeConnection(int client_fd) {
    std::vector<int>::iterator it = std::find(clients.begin(), clients.end(), client_fd);
    if (it != clients.end()) {
        ::close(client_fd);
        clients.erase(it);
    }
}