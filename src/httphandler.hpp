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
            void addRoute(SWS::HttpMethod method, std::string route, HandlerFunc func);
            SWS::HttpResponse handleRequest(const SWS::HttpRequest& request);
    };
}

#endif