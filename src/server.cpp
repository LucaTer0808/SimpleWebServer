#include "../includes/server.hpp"

SWS::Server::Server() : listening_socket(nullptr), conns(), handler() {}