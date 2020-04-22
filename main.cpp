#include <iostream>
#include <boost/program_options.hpp>
#include <seastar/core/reactor.hh>
#include <seastar/core/app-template.hh>
#include <seastar/core/distributed.hh>
#include <tcp/tcp_server.h>
#include <vertx/hazelcast_cluster.h>
#include <vertx/clustered_message.h>
#include <future>
#include <thread>
#include <mutex>
#include <vertx/vertx.h>
#include <seastar/core/thread.hh>
#include <seastar/core/sleep.hh>
#include <seastar/core/smp.hh>
#include <vertx/queue_server.h>

//using namespace seastar;
namespace bpo = boost::program_options;
using namespace std::chrono_literals;

int main(int ac, char** av) {
    hazelcast::client::ClientConfig config;
    hazelcast::client::Address a{"127.0.0.1", 5701 };
    config.getNetworkConfig().addAddress(a);
    hazelcast_cluster cluster {config};
    cluster.join();
    vertx::hz = std::make_unique<hazelcast_cluster>(cluster);
    vertx::port = 10000;

    vertx::consumer("tarcza", [] (clustered_message& msg, clustered_message& response) {
        std::string body = msg.getBody();
        std::cout << seastar::this_shard_id() << std::endl;

        response.setBody("dupa tam!");
    });

    seastar::app_template app;
    app.add_options()("port", bpo::value<uint16_t>()->default_value(vertx::port), "tcp server port");


    return app.run_deprecated(ac, av, [&] {
        auto&& config = app.configuration();
        uint16_t port = config["port"].as<uint16_t>();

        seastar::seastar_logger.set_level(seastar::log_level::trace);



        std::cout<< "seastar::smp::count " << seastar::smp::count << std::endl;

        auto q_server = new seastar::distributed<queue_server>;
        uint16_t q_port = 1234;
        (void)q_server->start().then([q_server = std::move(q_server), q_port] () mutable {
            seastar::engine().at_exit([q_server] {
                return q_server->stop();
            });
            (void)q_server->invoke_on_all( &queue_server::listen, seastar::ipv4_addr{q_port});
        }).then([q_port] {
            seastar::seastar_logger.info("queue_server server listening on port {}", q_port);
        });


        auto server = new seastar::distributed<tcp_server>;
        (void)server->start().then([server = std::move(server), port] () mutable {
            seastar::engine().at_exit([server] {
                return server->stop();
            });
            (void)server->invoke_on_all(&tcp_server::listen, seastar::ipv4_addr{port});
        }).then([port] {
            seastar::seastar_logger.info("vertx_tcp server listening on port {}", port);
        });



    });
}
