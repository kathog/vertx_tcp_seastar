//
// Created by nerull on 13.04.2020.
//

#include "tcp/tcp_server.h"
#include "tcp/connection.h"

using namespace std::chrono_literals;

seastar::future<> tcp_server::listen(seastar::ipv4_addr addr) {
    {
        seastar::listen_options lo;
        lo.reuse_address = true;
        lo.lba = seastar::server_socket::load_balancing_algorithm::port;
        _tcp_listeners.push_back(seastar::listen(make_ipv4_address(addr), lo));
        do_accepts(_tcp_listeners);
    }
    {
        seastar::listen_options lo;
        lo.proto = seastar::transport::SCTP;
        lo.reuse_address = true;
        lo.lba = seastar::server_socket::load_balancing_algorithm::port;
        _sctp_listeners.push_back(seastar::listen(make_ipv4_address(addr), lo));
        do_accepts(_sctp_listeners);
    }
    return seastar::make_ready_future<>();
}

seastar::future<> tcp_server::stop() {
    //todo: zatrzymanie pracy listenerów
    return seastar::make_ready_future<>();
}

void tcp_server::do_accepts(std::vector<seastar::api_v2::server_socket> &listeners) {
    (void)listeners[0].accept().then_wrapped([this, &listeners] (seastar::future<seastar::accept_result> f_ar) mutable {

        if (f_ar.failed()) {
            f_ar.ignore_ready_future();
            return ;
        }

        auto ar = f_ar.get0();
        auto conn = new connection(*this, std::move(ar.connection), std::move(ar.remote_address));
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