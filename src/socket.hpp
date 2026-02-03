#ifndef socket_hpp
#define socket_hpp

#include <stdio.h>
#include <cstdint>
#include <vector>
#include <memory>

#include "connection.hpp"

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
             * Accepts a new connection and returns the corresponding connection object.
             */
            std::unique_ptr<SWS::Connection> accept();

            /**
             * Closes the socket
             */
            void close();
            
        private:
            int socket_fd;
    };
}

#endif /* socket_hpp*/