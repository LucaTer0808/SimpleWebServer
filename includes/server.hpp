#ifndef server_hpp
#define server_hpp

#include <unordered_map>
#include <connection.hpp>
#include <memory>

#include "socket.hpp"

namespace SWS {
    class Server {
        private:
            SWS::Socket listening_socket;
            std::unordered_map<int, std::unique_ptr<SWS::Connection>> conns;

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


    };
}

#endif /* server_hpp */