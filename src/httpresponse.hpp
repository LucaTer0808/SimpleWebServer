#ifndef httpresponse_hpp
#define httpresponse_hpp

#include <string>

namespace SWS {
    class HttpResponse {
        private:
            int statusCode;
            std::string statusText;
            std::string body;
            std::string httpVersion = "HTTP/1.1";

        public:
            HttpResponse(int statusCode, std::string statusText, std::string bode);
            std::string serialize() const;
    };
}

#endif /* httpresponse_hpp */