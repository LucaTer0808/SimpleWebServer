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
            static constexpr int OK = 200;
            static constexpr int NOT_FOUND = 404;

            /**
             * Constructor for a HttpReponse. Takes a status code, the corresponding text and a body.
             */
            HttpResponse(int statusCode, std::string statusText, std::string body);

            /**
             * Serializes the content of a Response to a string to make it processible.
             */
            std::string serialize() const;
    };
}

#endif /* httpresponse_hpp */