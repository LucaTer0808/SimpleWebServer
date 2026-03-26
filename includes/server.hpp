#ifndef server_hpp
#define server_hpp

#include <unordered_map>
#include <memory>
#include <functional>

#include "socket.hpp"
#include "eventhandler.hpp"
#include "http/httphandler.hpp"
#include "http/httpmethod.hpp"
#include "http/httpresponse.hpp"
#include "http/httprequest.hpp"

namespace SWS {
    class Server {
        private:
            using HandlerFunc = std::function<HttpResponse(const HttpRequest&)>;

            std::unique_ptr<SWS::Socket> listening_socket; // to let it also be null
            SWS::EventHandler event_handler;
            std::unordered_map<int, std::unique_ptr<SWS::Connection>> conns;
            SWS::HttpHandler http_handler;

        public:
            /**
             * @brief Constructor for a Server object. Since all other objects are constructed via the basic constructor, we may just use the default constructor here.
             */
            Server() = default;

            /**
             * @brief Desctructor for an existing Server object. We can just use the default destructor.
             */
            ~Server() = default;

            /**
             * Both the copy instructor and the copy assing operator are disabled!.
             */
            Server(const Server&) = delete;
            Server& operator=(const Server&) = delete;

            /**
             * @brief Declaration of the move constructor. Just use the default move constructor.
             * @param other The temporary Server object to move from.
             */
            Server(Server&& other) = default;

            /**
             * @brief Declaration of move operator. Just use the default move operator.
             * @param other The Server object to move from.
             */
            Server& operator=(Server&& other) = default;

        public:
            /**
            * @brief Registers a GET-Endpoint.
            * @param route The Route to address.
            * @param func The function to execute. Can be defined inline which is cool. 
            */
            void get(std::string route, HandlerFunc func);

            /**
             * @brief Start up the server. Before starting, all routes have to be added to ensure functionality!
             * @param port The port to listen on.
             * @return LISTENING, if the server started properly.
             * @return FAILURE, if the start of the webserver did not work.
             */
            SWS::ServerStatus start(uint16_t port);

            /**
             * @brief The main loop for the master thread responsible for managing connections and distributing orders.
             */
            void master_thread_loop();
    };

    enum class ServerStatus {
        LISTENING, FAILURE
    };
}

#endif /* server_hpp */