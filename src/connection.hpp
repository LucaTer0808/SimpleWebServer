#ifndef connection_hpp
#define connection_hpp

#include <string>

namespace SWS {
    class Connection {
        private:
            int client_fd = -1;

        public:

            /**
             * Constructor for a connection object.
             */
            Connection(const int socket_fd);

            /**
             * Desctructor for a connection object.
             */
            ~Connection();

            /**
             * Forbids copying the connectio object
             */
            Connection(const Connection&) = delete;
            /**
             * Forbids the copying operator aswell.
             */
            Connection& operator=(const Connection&) = delete;

            /**
             * Explicit declaration of the move operation.
             */
            Connection(Connection&& other) noexcept;
            /**
             * Explicit declaration of the move operator.
             */
            Connection& operator=(Connection&& other) noexcept;

            /**
             * Sending data to the socket.
             */
            void send(const std::string& data);
            
            /**
             * Receiving data from the socket.
             */
            std::string receive();

            /**
             * Closing the connection to the socket.
             */
            void close();

    };
}

#endif /* connection_hpp */