#include "httphandler.hpp"

void SWS::HttpHandler::addRoute(SWS::HttpMethod method, std::string route, HandlerFunc func) {
    std::pair<SWS::HttpMethod, std::string> key = std::make_pair(method, std::move(route));
    this->routes.emplace(std::move(key), std::move(func));
}

SWS::HttpResponse SWS::HttpHandler::handleRequest(const SWS::HttpRequest& request) {
    std::pair<SWS::HttpMethod, std::string> key = std::make_pair(request.getMethod(), request.getPath());
    
    auto it = this->routes.find(key);

    if (it != this->routes.end()) {
        HandlerFunc& func = it->second;
        return func(request);
    } else {
        return SWS::HttpResponse(404, "Not Found", "");
    }
}