#ifndef connection_hpp
#define connection_hpp

#include <string>
#include "connection_status.hpp"


namespace SWS {
    class Connection {
        private:
            static constexpr int MAXIMUM_BUFFER_SIZE = 64 * 1024; // 64 kb

            int client_fd = -1;
            size_t bytes_sent_offset = 0; // to increase sending speed
            std::string buffer_out = "";
            std::string buffer_in = "";

            /**
             * @brief Finds the index of the beginning of the \r\n\r\n sequence that indicates the end of a request.
             * @return The index as size_t or std::string::npos, if no substring with said sequence was found.
             */
            size_t find_request_ending() const;

        public:

            /**
             * Constructor for a connection object. Note, that due to non-blocking purposes, the fd has
             * to identify an existing client socket already!
             * @param client_fd The file descriptor identifiying the connection.
             */
            Connection(const int client_fd);

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
             * @param other The temporary object to move from.
             */
            Connection(Connection&& other) noexcept;
            
            /**
             * Explicit declaration of the move operator.
             * @param other The object to move assign from.
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
             * @return ERROR, when an error occured while pushing data through the socket.
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
             * @brief Removes the latest full request from the buffer and returns it.
             * @return The whole request as a string or "", if the buffer does not contain a full request.
             */
            std::string get_latest_request();

            /**
             * Closing the connection to the socket.
             */
            void close();

    };
}

#endif /* connection_hpp */