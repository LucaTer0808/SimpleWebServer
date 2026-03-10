#ifndef connection_hpp
#define connection_hpp

#include <string>
#include "connection_status.hpp"

static constexpr int MAXIMUM_BUFFER_SIZE = 65536;

namespace SWS {
    class Connection {
        private:
            int client_fd = -1;
            std::string buffer_out;
            std::string buffer_in;

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
             * Forbids copying the connection object
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
             * Appends data to be send to the output puffer and immediately tries to push it through the channel.
             * @param data The data to be appended to the output puffer.
             * @return WANT_WRITE, when there still is data in the puffer to be pushed through the socket after the first initial send.
             * @return COMPLETE, when all data could be pushed through the socket and nothing is left to do.
             */
            SWS::ConnectionStatus send(const std::string& data);

            /**
             * @brief Tries to push the data from the output buffer through the channel to the client socket.
             * @return WANT_WRITE, when the buffer still contains data to be pushed through.
             * @return COMPLETE, when the whole content from the buffer could be pushed through the channel.
             */
            SWS::ConnectionStatus push_data();
            
            /**
             * @brief Reveives data from the client socket and writes it to the input buffer.
             * @return OPEN, when the data has been received properly.
             * @return CLOSED, when the socket was closed by the client.
             * @return ERROR, when an unfixable error occured.
             * @return PAYLOAD_TOO_LARGE, when the input buffer exceeds the limit that is considered healthy.
             */
            SWS::ConnectionStatus receive();

            /**
             * Closing the connection to the socket.
             */
            void close();

    };
}

#endif /* connection_hpp */