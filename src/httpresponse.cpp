#include <stdexcept>

#include "httpresponse.hpp"

SWS::HttpResponse::HttpResponse(int statusCode, std::string statusText, std::string body) : statusCode(statusCode), statusText(statusText), body(body) {
    if (statusCode < 0) {
        throw std::runtime_error("The given StatusCode " + std::to_string(statusCode) + " is invalid!");
    }
}

std::string SWS::HttpResponse::serialize() const {
    return this->httpVersion + " " + std::to_string(this->statusCode) + " " + this->statusText + " " + this->body; 
}