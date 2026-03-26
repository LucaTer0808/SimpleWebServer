#include <sys/epoll.h>
#include <stdexcept>
#include <unistd.h>

#include "eventhandler.hpp"
#include "common/log.hpp"

SWS::EventHandler::EventHandler() : epoll_fd(epoll_create1(EPOLL_CLOEXEC)) {
    if (epoll_fd == -1) {
        SWS::log_errno("Creating an epoll instance has failed! Event polling is not possible!");
        throw std::runtime_error("epoll_create1(EPOLL_CLOEXEC) failed!");
    }
}

SWS::EventHandler::~EventHandler() {
    this->close();
}

SWS::EventHandler::EventHandler(EventHandler&& other) noexcept {
    this->epoll_fd = other.epoll_fd;
    other.epoll_fd = -1;   
}

SWS::EventHandler& SWS::EventHandler::operator=(EventHandler&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    this->close();

    this->epoll_fd = other.epoll_fd;
    other.epoll_fd = -1;

    return *this;
}

SWS::EventHandlerStatus SWS::EventHandler::add(int fd, uint32_t events) {
    if (fd < 0) {
        SWS::log(SWS::LogLevel::WARNING, std::string("Negative fd passed to the EventHandler. No Socket can be registered!"));
        return SWS::EventHandlerStatus::FAILURE;
    }

    epoll_event ev{};
    ev.events = events;
    ev.data.fd = fd;

    int result = epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if (result < 0) {
        SWS::log_errno(std::string("Adding the socket with fd: ") + std::to_string(fd) + std::string(" for event polling failed!"));
        return SWS::EventHandlerStatus::FAILURE;
    }

    return SWS::EventHandlerStatus::SUCCESS;
}

SWS::EventHandlerStatus SWS::EventHandler::edit(int fd, uint32_t events) {
    if (fd < 0) {
        SWS::log(SWS::LogLevel::WARNING, std::string("Negative fd passed to the EventHandler. No Socket can be edited!"));
        return SWS::EventHandlerStatus::FAILURE;
    }

    epoll_event ev{};
    ev.events = events;
    ev.data.fd = fd;

    int result = epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, fd, &ev);
    if (result < 0) {
        SWS::log_errno(std::string("Editing the socket with fd: ") + std::to_string(fd) + std::string(" failed!"));
        return SWS::EventHandlerStatus::FAILURE;
    }

    return SWS::EventHandlerStatus::SUCCESS;
}

SWS::EventHandlerStatus SWS::EventHandler::remove(int fd) {
    if (fd < 0) {
        SWS::log(SWS::LogLevel::WARNING, std::string("Negative fd passed to the EventHandler. No Socket can be removed!"));
        return SWS::EventHandlerStatus::FAILURE;
    }

    int result = epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, nullptr);

    if (result < 0) {
        SWS::log_errno(std::string("Removing the event polling for socket with fd: ") + std::to_string(fd) + std::string(" has failed!"));
        return SWS::EventHandlerStatus::FAILURE;
    }

    return SWS::EventHandlerStatus::SUCCESS;
}

void SWS::EventHandler::close() {
    if (this->epoll_fd >= 0) {
        ::close(epoll_fd);
        this->epoll_fd = -1;
    }
}