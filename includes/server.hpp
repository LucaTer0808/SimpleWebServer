#ifndef server_hpp
#define server_hpp

#include <unordered_map>
#include <connection.hpp>
#include <memory>
#include <functional>

#include "socket.hpp"
#include "http/httphandler.hpp"
#include "http/httpmethod.hpp"
#include "http/httpresponse.hpp"
#include "http/httprequest.hpp"

namespace SWS {
    class Server {
        private:
            using HandlerFunc = std::function<HttpResponse(const HttpRequest&)>;

            SWS::Socket listening_socket;
            std::unordered_map<int, std::unique_ptr<SWS::Connection>> conns;

            SWS::HttpHandler handler;

        public:
            /**
             * @brief Constructor for a Server object.
             * @param port The port the listening socket operates on.
             */
            Server(uint16_t port);

            /**
             * @brief Desctructor for an existing Server object.
             */
            ~Server();

            /**
             * Both the copy instructor and the copy assing operator are disabled!.
             */
            Server(const Server&) = delete;
            Server& operator=(const Server&) = delete;

            /**
             * @brief Declaration of the move constructor.
             * @param other The temporary Server object to move from.
             */
            Server(const Server&& other);

            /**
             * @brief Declaration of move operator.
             * @param other The Server object to move from.
             */
            Server& operator=(const Server&& other);

        public:
            /**
             * @brief Specifies a route with a http method and a function to execute once that route is hit!
             * @param http_method The desired HTTP method, such as GET, POST etc...
             * @param route The route the user has to send a request to in order to trigger that endpoint.
             * @param func The function to execute.
             */
            void add_route(SWS::HttpMethod http_method, std::string route, HandlerFunc func);
    };
}

#endif /* server_hpp */