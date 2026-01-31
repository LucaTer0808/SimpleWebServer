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
            SWS::HttpMethod getMethod() const;
            std::string getPath() const;
    };
}

#endif /* httprequest_hpp */