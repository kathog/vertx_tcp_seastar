//
// Created by nerull on 13.04.2020.
//

#ifndef VERTX_TCP_SEASTAR_TCP_SERVER_H
#define VERTX_TCP_SEASTAR_TCP_SERVER_H
#include <seastar/core/reactor.hh>
#include <seastar/core/app-template.hh>
#include <seastar/core/temporary_buffer.hh>
#include <seastar/core/distributed.hh>
#include <seastar/core/print.hh>
#include <seastar/util/log.hh>
#include "client_connection.h"
#include <vertx/clustered_message.h>

#include <vector>
#include <seastar/net/api.hh>

//using namespace seastar;
using namespace seastar::net;
using namespace std::chrono_literals;


class tcp_server {

private:
    std::vector<seastar::api_v2::server_socket> _tcp_listeners;
    std::vector<seastar::api_v2::server_socket> _sctp_listeners;

public:

    seastar::future<> listen(seastar::ipv4_addr addr);
    seastar::future<> stop();
    void do_accepts(std::vector<seastar::api_v2::server_socket>& listeners);
    using f_invoke = seastar::future<> (*)(client_connection *conn, clustered_message msg);
    std::unordered_map<std::string, f_invoke> functions;
};


#endif //VERTX_TCP_SEASTAR_TCP_SERVER_H
