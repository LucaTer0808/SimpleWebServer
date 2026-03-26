#pragma once /* eventhandler_hpp */

#include <vector>

namespace SWS {
    class EventHandler {
        public:
            /**
             * Constructor for an EventHandler;
             */
            EventHandler();

            /**
             * Desctructor for an EventHandler;
             */
            ~EventHandler();

            /**
             * Since we use RAII, copying an EventHandler is strictly forbidden.
             */
            EventHandler(const EventHandler&) = delete;
            EventHandler operator=(const EventHandler&) = delete;

            /**
             * @brief Move constructor for an EventHandler.
             * @param other The temporary EventHandler to moveconstruct from.
             */
            EventHandler(EventHandler&& other) noexcept;

            /**
             * @brief Move operator for an EventHandler.
             * @param other The EventHandler to move from.
             */
            EventHandler& operator=(EventHandler&& other) noexcept;

            /**
             * Returns a vector containing file desctiptors of all sockets who could perform IO.
             * Blocks until at least on event occurs.
             */
            std::vector<int> wait_events();

            /**
             * @brief Add a file descriptor representing a socket for event polling.
             * @param fd The file descriptor in question.
             * @param events The events connected by |.
             * @return FAILURE, if a negative fd was passed or the registration of a socket represented by a valid fd failed.
             * @return SUCCESS, if the socket represented by the fd was registered properly.
             */
            SWS::EventHandlerStatus add(int fd, uint32_t events);

            /**
             * @brief Editing the events polled for the socket with the given fd.
             * @param fd The file descriptor in question.
             * @param events The new events connected with |.
             */
            SWS::EventHandlerStatus edit(int fd, uint32_t events);

            /**
             * @brief Removes a file descriptor representing a socket from event polling, e.g. when it is closed.
             * @param fd The file descriptor representing the socket.
             */
            SWS::EventHandlerStatus remove(int fd);
        
        private:
            int epoll_fd;

            /**
             * Closes the epoll instance represented by the file descriptor.
             */
            void close();
    };

    enum class EventHandlerStatus {
        SUCCESS, FAILURE
    };
}