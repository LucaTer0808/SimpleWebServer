#include "httprequest.hpp"

SWS::HttpMethod SWS::HttpRequest::getMethod() const {
    return this->method;
}

std::string SWS::HttpRequest::getPath() const {
    return this->path;
}