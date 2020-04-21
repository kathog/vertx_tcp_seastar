//
// Created by nerull on 19.04.2020.
//

#ifndef VERTX_TCP_SEASTAR_CLIENT_CONNECTION_H
#define VERTX_TCP_SEASTAR_CLIENT_CONNECTION_H
#include <iostream>
#include <seastar/core/app-template.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/future-util.hh>
#include <seastar/core/distributed.hh>
#include <vertx/clustered_message.h>

//using namespace seastar;
using namespace seastar::net;
using namespace std::chrono_literals;

class client_connection {
public:
    client_connection(seastar::connected_socket&& fd) : _fd(std::move(fd)), _read_buf(_fd.input()), _write_buf(_fd.output()) {}

    seastar::future<> do_write(char * msg, int index) {
        return _write_buf.write(msg, index).then([this] {
            return _write_buf.close().then([this](){
                return seastar::make_ready_future<>();
            });
        }).then([this] {
                return seastar::make_ready_future();
        });
    }

private:
    seastar::connected_socket _fd;
    seastar::input_stream<char> _read_buf;
    seastar::output_stream<char> _write_buf;
    size_t _bytes_read = 0;
    size_t _bytes_write = 0;

};


#endif //VERTX_TCP_SEASTAR_CLIENT_CONNECTION_H
