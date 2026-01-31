#ifndef httphandler_hpp
#define httphandler_hpp

#include <map>
#include <string>
#include <functional>
#include <utility>

#include "httprequest.hpp"
#include "httpresponse.hpp"
#include "httpmethod.hpp"

namespace SWS {
    class HttpHandler {
        using HandlerFunc = std::function<HttpResponse(const HttpRequest&)>;

        private:
            std::map<std::pair<SWS::HttpMethod, std::string>, HandlerFunc> routes;

        public:
            /**
             * Adds a new route with a HttpMethod and links it to a function to be called when said endpoint is hit.
             */
            void addRoute(SWS::HttpMethod method, std::string route, HandlerFunc func);

            /**
             * Handles an incoming request by orchestrating it to the correct handler function.
             */
            SWS::HttpResponse handleRequest(const SWS::HttpRequest& request);
    };
}

#endif