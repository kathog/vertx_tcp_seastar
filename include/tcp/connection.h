//
// Created by nerull on 13.04.2020.
//

#ifndef VERTX_TCP_SEASTAR_CONNECTION_H
#define VERTX_TCP_SEASTAR_CONNECTION_H
#include <seastar/core/reactor.hh>
#include <seastar/core/app-template.hh>
#include <seastar/core/temporary_buffer.hh>
#include <seastar/core/distributed.hh>
#include <seastar/core/print.hh>
#include <seastar/util/log.hh>
#include <seastar/core/ragel.hh>
#include <string>
#include "tcp_server.h"
#include "client_connection.h"

//using namespace seastar;

class request_parser : public seastar::ragel_parser_base<request_parser> {
private:

    bool _done = false;

public:
    void init() {
        init_base();
    }

    char* parse(char* p, char* pe, char* eof) {
        seastar::sstring_builder::guard g(_builder, p, pe);
        g.mark_start(p);
        g.mark_end(pe);
        if (strlen(p) == 0 || strlen(pe) > 0 || strlen(p) % 1024 != 0) {
            _done = true;
        }
        return pe;
    }

    std::string get_data () {
        return get_str();
    }

    bool done () {
        return _done;
    }

};



class connection {
public:
    connection(tcp_server &server, seastar::connected_socket &&fd, seastar::socket_address addr);

    seastar::future<> process();
    seastar::future<> read0();

private:
    seastar::connected_socket _fd;
    seastar::input_stream<char> _read_buf;
    seastar::output_stream<char> _write_buf;
    std::stringstream request;
    int request_size;
    int fully_size;
    request_parser _parser;
    using f_invoke = seastar::future<> (*)(client_connection *conn, clustered_message msg);
    std::unordered_map<std::string, f_invoke> functions;
    std::string message;
};


#endif //VERTX_TCP_SEASTAR_CONNECTION_H
