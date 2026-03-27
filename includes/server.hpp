#ifndef server_hpp
#define server_hpp

#include <unordered_map>
#include <memory>
#include <functional>
#include <queue>
#include <tuple>
#include <mutex>
#include <condition_variable>
#include <thread>

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

            std::queue<std::tuple<int, std::string>> jobs;
            std::mutex jobs_mutex;
            std::condition_variable queue_not_empty;

            std::queue<std::tuple<int, std::string>> responses;
            std::mutex responses_mutex;

            std::vector<std::jthread> worker_threads;

            std::unique_ptr<SWS::Socket> listening_socket; // to let it also be null
            SWS::EventHandler event_handler;
            std::unordered_map<int, std::unique_ptr<SWS::Connection>> conns;
            SWS::HttpHandler http_handler;

        public:
            static constexpr size_t THREAD_MULT = 3;
    
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

            /**
             * @brief Appends an incoming job to the jobs queue to be consumed by the worker threads. A lock has to be acquired and
             * a waiting process has to be notified.
             * @param fd The file descriptor of the connection sending the job and also receiving the answer.
             * @param request_string The request as a string.
             */
            void append_job(int fd, std::string request_string);

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
             * @param num_workers The amount of worker threads for increased concurrency. If none is passed, we multiply the value
             * std::thread::hardware_concurrency delivers by a fixed constant. std::thread::hardware_concurrency also works as the lower bound
             * for the amunt of threads running to at least make use of all cores. 0 by deafult.
             */
            void start(uint16_t port, size_t num_workers = 0);

            /**
             * @brief The main loop for the master thread responsible for managing connections and distributing orders.
             */
            void master_thread_loop();

            /**
             * @brief The main loop for any worker thread responsible for actually serving requests. It keeps checking if the queue is not empty
             * and continues to consume and serve requests until it is empty. It then makes the thread wait on the condition variable until it is needed again.
             */
            void worker_thread_loop();

            /**
             * Calculates the required number of threads as stated in the start()-method;
             * @param num_workers The desired number of worker threads or 0 if left basic.
             */
            size_t calculate_thread_number(size_t num_workers);
    };
}

#endif /* server_hpp */