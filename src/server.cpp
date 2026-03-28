#include "../includes/server.hpp"
#include "common/log.hpp"

SWS::Server::Server() : jobs(), worker_threads(), listening_socket(nullptr), conns(), event_handler(), http_handler() {}

void SWS::Server::get(std::string route, HandlerFunc func) {
    std::string route_copy = route;
    SWS::HttpHandlerStatus status = this->http_handler.addRoute(SWS::HttpMethod::GET, std::move(route), std::move(func));

    if (status == SWS::HttpHandlerStatus::EXISTS) {
        SWS::log(SWS::LogLevel::WARNING, std::string("The registration of the GET Method with Route ") + route_copy + std::string(" did not work. Most likely, the route is registered alredy!"));
    }
}

void SWS::Server::start(uint16_t port, size_t num_workers = 0) {
    try {
        this->listening_socket = std::make_unique<SWS::Socket>(port);
    } catch (std::runtime_error &error) {
        SWS::log(SWS::LogLevel::ERROR, std::string("The SimpleWebServer could not be started!"));
        return;
    }

    size_t actual_concurrency = this->calculate_thread_number(num_workers);

    for (size_t i = 0; i < actual_concurrency; ++i) {
        this->worker_threads.emplace_back([this]() {
            this->worker_thread_loop();
        });
    }

    SWS::log(SWS::LogLevel::INFO, std::string("SimpleWebServer is now listening on port ") + std::to_string(port));
    this->master_thread_loop();
}

// TODO: Implement
void SWS::Server::master_thread_loop() {
    while(true) {
        std::unordered_map<int, uint32_t> events = this->event_handler.wait_events();

        for (auto& [fd, event_mask] : events) {
            if (this->conns.find(fd) == this->conns.end()) {
                SWS::log(SWS::LogLevel::WARNING, std::string("No connetion represented by fd: " + std::to_string(fd) + std::string(" currently exists. The events can not be executed!")));
                continue;
            }
        }
    }

}

// TODO: Implement
void SWS::Server::worker_thread_loop() {
    while(true) {}
}

void SWS::Server::append_job(int fd, std::string request_string) {
    if (fd < 0) {
        SWS::log(SWS::LogLevel::WARNING, "Negative file descriptor passed to append_job. Can not append the job!");
        return;
    }

    if (this->conns.find(fd) == this->conns.end()) {
        SWS::log(SWS::LogLevel::WARNING, "The given file descriptor does not represent a valid connection! append_job cannot be called!");
        return;
    }

    std::promise<std::string> promise;
    std::future<std::string> future = promise.get_future();

    this->conns.at(fd)->enqueue_future(std::move(future));

    SWS::Job job;
    job.request = std::move(request_string);
    job.promise = std::move(promise);

    this->jobs.push(std::move(job));
}

size_t SWS::Server::calculate_thread_number(size_t num_workers) {
    size_t actual_concurrency = 0;
    size_t min_concurrency = std::thread::hardware_concurrency();

    if (num_workers == 0) {
        actual_concurrency = min_concurrency * SWS::Server::THREAD_MULT;
    } else {
        if (num_workers > min_concurrency) {
            actual_concurrency = num_workers;
        } else {
            actual_concurrency = min_concurrency;
        }
    }

    return actual_concurrency;
}

