//
// Created by nerull on 21.04.2020.
//

#ifndef VERTX_TCP_SEASTAR_QUEUE_SERVER_H
#define VERTX_TCP_SEASTAR_QUEUE_SERVER_H
#include <seastar/core/thread.hh>
#include <seastar/core/seastar.hh>

class queue_server;

class queue_connect {
public:
    queue_connect (queue_server& server, seastar::connected_socket&& fd, seastar::socket_address addr) :
            _fd(std::move(fd)),
            _read_buf(_fd.input({8 * 1024, 512, 128 * 1024})),
            _write_buf(_fd.output(4 * 1024)) {};

    seastar::future<> process () {
        return _read_buf.close().then([this]() {
            return _write_buf.close().then([this]() {
                std::string message;
                {
                    std::unique_lock<std::mutex> lock(cm::queue_mutex);
                    cm::condition.wait(lock, [] { return !cm::messages.empty(); });
                    if (!cm::messages.empty()) {
                        message = std::move(cm::messages.front());
                        cm::messages.pop();
                    }
                }


//                return seastar::async([this, &message] () {
                return to_message(message).then([this](clustered_message &&request_message) {
                        auto function_invoke = vertx::consumers[request_message.getAddress()];
                        clustered_message response_message{0, 1, 9, true, request_message.getAddress(), request_message.getReplay(), vertx::port,request_message.getHost(), 4, ""};
                        function_invoke(request_message, response_message);

                        return to_string(response_message).then([this, request_message = std::move(request_message)](std::string &&string_message) {
//                        const char *c_host = request_message.getHost() == "localhost" ? "127.0.0.1" : request_message.getHost().c_str();
                            seastar::socket_address local = seastar::socket_address(::sockaddr_in{AF_INET, INADDR_ANY, {0}});
                            return connect(seastar::make_ipv4_address({request_message.getHost().data(), (uint16_t) request_message.getPort()}), local, seastar::transport::TCP).then([this, string_message = std::move(string_message)](seastar::connected_socket fd) {
                                auto conn = new client_connection(std::move(fd));
                                return conn->do_write(static_cast<std::string>(string_message).data(), string_message.size()).then_wrapped([conn](auto &&f) {
                                    delete conn;
                                    try {
                                        f.get();
                                    } catch (std::exception &ex) {
                                        seastar::fprint(std::cerr, "request error: %s\n", ex.what());
                                    }
                                    return seastar::make_ready_future<>();
                                });
                            });
                        });
//                    });
                });

            });
        });
    }

private:
    seastar::connected_socket _fd;
    seastar::input_stream<char> _read_buf;
    seastar::output_stream<char> _write_buf;

};


class queue_server {
public:
    void listen(seastar::ipv4_addr addr) {
        {
            seastar::listen_options lo;
            lo.reuse_address = true;
            lo.lba = seastar::server_socket::load_balancing_algorithm::port;
            _tcp_listeners.push_back(seastar::listen(make_ipv4_address(addr), lo));
            do_accepts(_tcp_listeners);
        }
    }

private:
    void do_accepts(std::vector<seastar::api_v2::server_socket> &listeners) {
        (void)listeners[0].accept().then_wrapped([this, &listeners] (seastar::future<seastar::accept_result> f_ar) mutable {

            if (f_ar.failed()) {
                f_ar.ignore_ready_future();
                return ;
            }

            auto ar = f_ar.get0();

            auto conn = new queue_connect(*this, std::move(ar.connection), std::move(ar.remote_address));
            (void)conn->process().then_wrapped([conn] (auto&& f) {
                delete conn;
                try {
                    f.get();
                } catch (std::exception& ex) {
                    seastar::seastar_logger.error("request error {}" , ex.what());
                }
            });
            do_accepts(listeners);
        }).then_wrapped([] (auto&& f) {
            try {
                f.get();
            } catch (std::exception& ex) {
                seastar::seastar_logger.error("accept failed {}" , ex.what());
            }
        });
    }


    std::vector<seastar::api_v2::server_socket> _tcp_listeners;



};


#endif //VERTX_TCP_SEASTAR_QUEUE_SERVER_H
