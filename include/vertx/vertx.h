//
// Created by nerull on 20.04.2020.
//

#ifndef VERTX_TCP_SEASTAR_VERTX_H
#define VERTX_TCP_SEASTAR_VERTX_H
#include <vertx/hazelcast_cluster.h>

namespace vertx {

    inline std::unique_ptr<hazelcast_cluster> hz;
    inline int port;
    inline int loop;
    inline std::vector<std::thread> workers;
    inline std::unordered_map<std::string, void (*)(clustered_message&, clustered_message&)> consumers;

    void consumer (std::string&& address, void function(clustered_message&, clustered_message&) ) {
        consumers.emplace(address, function);
        std::string host = "127.0.0.1";
        hz->addSub(address, vertx::port, host);
    }


//    seastar::future<> invoke_on_message(clustered_message &&message) {
//        struct sockaddr_in serv_addr;
//        auto function_invoke = consumers[message.getAddress()];
//
//        if (function_invoke) {
//            clustered_message response_message{0, 1, 9, true, message.getAddress(), message.getReplay(), port, message.getHost(), 4, ""};
//            function_invoke(message, response_message);
//
//            std::string s_message;
//            s_message.resize(2048);
//            int index = 0;
//            auto time = std::chrono::high_resolution_clock::now();
//            static_cast<clustered_message>(response_message).to_cstring(s_message.data(), index);
//            auto end = std::chrono::high_resolution_clock::now();
//            auto tResult = std::chrono::duration_cast<std::chrono::nanoseconds>(end - time);
//
//            if (int sock = socket(AF_INET, SOCK_STREAM, 0); sock >= 0) {
//                serv_addr.sin_family = AF_INET;
//                serv_addr.sin_port = htons(message.getPort());
//                const char *c_host = message.getHost() == "localhost" ? "127.0.0.1" : message.getHost().c_str();
//                if (inet_pton(AF_INET, c_host, &serv_addr.sin_addr) > 0) {
//                    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) >= 0) {
//                        size_t send_bytes = send(sock, s_message.data(), index, 0);
//                    }
//                }
//                close(sock);
//            }
//        }
//        return seastar::make_ready_future<>();
//    }

//    void register_workers() {
//        for (int i = 0; i < 12; i++) {
//            workers.emplace_back([]() {
//                for (;;) {
//                    clustered_message message;
//                    {
//                        std::unique_lock<std::mutex> lock(cm::queue_mutex);
//                        cm::condition.wait(lock, [] { return !cm::messages.empty(); });
//                        if (!cm::messages.empty()) {
//                            message = std::move(cm::messages.front());
//                            cm::messages.pop();
//                        }
//                    }
//                    invoke_on_message(std::move(message));
//                }
//            });
//        }
//    }

}


#endif //VERTX_TCP_SEASTAR_VERTX_H
