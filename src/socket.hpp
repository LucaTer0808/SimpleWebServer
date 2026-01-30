#ifndef socket_hpp
#define socket_hpp

#include <stdio.h>
#include <cstdint>
#include <vector>

namespace SWS { /* SimpleWebServer */
    class Socket {
        public:
            /**
             * Constructor for the creation of a socket. Creates the socket, binds it to the given port and listens on that port.
             */
            Socket(uint16_t port);

            /**
             * Descturctor for an existing socket. Closes all open connections and also closes the listening socket itself.
             */
            ~Socket();

            /**
             * Accepts a new connection and adds the representing file descriptor to the sockets clients vector.
             */
            int acceptConnection();

            /**
             * Closes the connection represented by the given file descriptor of the client.
             */
            void closeConnection(int client_fd);

        private:
            int socket_fd;
            std::vector<int> clients;
    };
}

#endif /* socket_hpp*/