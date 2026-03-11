#ifndef socket_hpp
#define socket_hpp

#include <stdio.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <unordered_map>

#include "connection.hpp"
#include "connection_status.hpp"

namespace SWS { /* SimpleWebServer */
    class Socket {
        public:
            /**
             * Constructor for the creation of a socket. Creates the socket, binds it to the given port and listens on that port.
             */
            Socket(const uint16_t port);

            /**
             * Descturctor for an existing socket. Closes all open connections and also closes the listening socket itself.
             */
            ~Socket();

            /**
             * Copy constructor is forbidden. Socket is a move-only type.
             */
            Socket(const Socket&) = delete;
            Socket& operator=(const Socket&) = delete;

            /**
             * Declaration for move constructor.
             */
            Socket(Socket&& other) noexcept;

            /**
             * Declaration of move assignment operator.
             */
            Socket& operator=(Socket&& other) noexcept;

            /**
             * Accepts a new connection and returns the status of said connection.
             * @return OPEN, when the new connection has been established properly.
             * @return WAITING, when there currently is no connection to establish! Important for non-blocking.
             * @return ERROR, when an error during while trying to establish the connection occured.
             */
            SWS::ConnectionStatus accept();

            /**
             * Closes the socket
             */
            void close();
            
        private:
            int socket_fd;
            std::unordered_map<int, std::unique_ptr<SWS::Connection>> clients;
    };
}

#endif /* socket_hpp*/