#include "../includes/server.hpp"
#include "common/log.hpp"

SWS::Server::Server() : listening_socket(nullptr), conns(), event_handler(), http_handler() {}

void SWS::Server::get(std::string route, HandlerFunc func) {
    std::string route_copy = route;
    SWS::HttpHandlerStatus status = this->http_handler.addRoute(SWS::HttpMethod::GET, std::move(route), std::move(func));

    if (status == SWS::HttpHandlerStatus::EXISTS) {
        SWS::log(SWS::LogLevel::WARNING, std::string("The registration of the GET Method with Route ") + route_copy + std::string(" did not work. Most likely, the route is registered alredy!"));
    }
}

SWS::ServerStatus SWS::Server::start(uint16_t port) {
    try {
        this->listening_socket = std::make_unique<SWS::Socket>(port);
    } catch (std::runtime_error &error) {
        return SWS::ServerStatus::FAILURE;
    }
    return SWS::ServerStatus::LISTENING;
}

void SWS::Server::master_thread_loop() {
    while(true) {

    }
}

