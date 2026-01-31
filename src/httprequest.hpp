#ifndef httprequest_hpp
#define httprequest_hpp

#include <string>

#include "httpmethod.hpp"

namespace SWS {
    class HttpRequest {
        private:
            SWS::HttpMethod method;
            std::string path;

        public:
            /**
             * Returns the HttpMethod of the request.
             */
            SWS::HttpMethod getMethod() const;

            /**
             * Returns the path of the request.
             */
            std::string getPath() const;
    };
}

#endif /* httprequest_hpp */