//
// Created by nerull on 13.04.2020.
//
#include <atomic>
#include <future>
#include "tcp/connection.h"
#include "vertx/clustered_message.h"
#include <seastar/core/thread.hh>

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

connection::connection(tcp_server& server, seastar::connected_socket&& fd, seastar::socket_address addr) :
    _fd(std::move(fd)),
    _read_buf(_fd.input({8 * 1024, 512, 128 * 1024})),
    _write_buf(_fd.output(4 * 1024)),
    request_size{1},
    request{""},
    fully_size{0}
    ,functions{server.functions}
    {};

seastar::future<> connection::process() {
    _parser.init();
    return read0();
}

const std::string response = R"eof(
HTTP/1.1 200 OK
content-type: application/json; charset=utf-8
content-length: 5238
strict-transport-security: max-age=15768000


{
  "itemIdList": [
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25840666"
        },
        "secType": [
          {
            "section": 173952,
            "root": 114869,
            "type": 7
          }
        ]
      },
      "score": "858092",
      "algDefId": "186"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25849197"
        },
        "secType": [
          {
            "section": 173972,
            "root": 78645,
            "type": 7
          }
        ]
      },
      "score": "755121",
      "algDefId": "186"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25842300"
        },
        "secType": [
          {
            "section": 173950,
            "root": 64945,
            "type": 7
          }
        ]
      },
      "score": "747042",
      "algDefId": "186"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25847489"
        },
        "secType": [
          {
            "section": 173952,
            "root": 114869,
            "type": 7
          }
        ]
      },
      "score": "669810",
      "algDefId": "186"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25847934"
        },
        "secType": [
          {
            "section": 166794,
            "root": 114869,
            "type": 14
          }
        ]
      },
      "score": "641406",
      "algDefId": "186"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25843799"
        },
        "secType": [
          {
            "section": 173953,
            "root": 114471,
            "type": 7
          }
        ]
      },
      "score": "598159",
      "algDefId": "186"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25845507"
        },
        "secType": [
          {
            "section": 173972,
            "root": 78645,
            "type": 7
          }
        ]
      },
      "score": "516964",
      "algDefId": "186"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25704851"
        },
        "secType": [
          {
            "section": 114883,
            "root": 114869,
            "type": 7
          }
        ]
      },
      "score": "1.8975338",
      "algDefId": "154"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25085885"
        },
        "secType": [
          {
            "section": 114883,
            "root": 114869,
            "type": 7
          }
        ]
      },
      "score": "1.1944994",
      "algDefId": "154"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25419526"
        },
        "secType": [
          {
            "section": 114883,
            "root": 114869,
            "type": 7
          }
        ]
      },
      "score": "1.0675685",
      "algDefId": "154"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25852096"
        },
        "secType": [
          {
            "section": 172690,
            "root": 114471,
            "type": 7
          }
        ]
      },
      "score": "609642",
      "algDefId": "186"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25101218"
        },
        "secType": [
          {
            "section": 114881,
            "root": 114869,
            "type": 7
          }
        ]
      },
      "score": "1.0806978",
      "algDefId": "154"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25842535"
        },
        "secType": [
          {
            "section": 166794,
            "root": 114869,
            "type": 14
          }
        ]
      },
      "score": "498019",
      "algDefId": "186"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25850116"
        },
        "secType": [
          {
            "section": 65081,
            "root": 64946,
            "type": 7
          }
        ]
      },
      "score": "539363",
      "algDefId": "186"
    },
    {
      "itemMetadata": {
        "objectDefinitionId": {
          "definitionOuterId": 1001,
          "objectId": "25700536"
        },
        "secType": [
          {
            "section": 114881,
            "root": 114869,
            "type": 7
          }
        ]
      },
      "score": "1.5017717",
      "algDefId": "154"
    }
  ],
  "widgetId": "39",
  "versionId": "51"
}
)eof";

seastar::future<> connection::read0() {

    if (_read_buf.eof()) {
        return seastar::make_ready_future();
    }

    return _read_buf.consume(_parser).then([this] () {
        if (!_parser.done()) {
            return read0();
        }
//        std::cout << "read0: " <<  seastar::this_shard_id() << std::endl;
        this->message = _parser.get_data();
        return _write_buf.write("").then([this] {
                return _write_buf.close();
            }).then([this] {
                {
                    std::unique_lock<std::mutex> lock(cm::queue_mutex);
                    cm::messages.push(this->message);
                }
                cm::condition.notify_one();

                seastar::socket_address local = seastar::socket_address(::sockaddr_in{AF_INET, INADDR_ANY, {0}});
                (void) connect(seastar::make_ipv4_address({"127.0.0.1", 1234}), local, seastar::transport::TCP).then(
                        [this](seastar::connected_socket fd) {
                            fd = std::move(fd);
                        });
                return seastar::make_ready_future();
            });
    }).handle_exception([] (std::exception_ptr exc) {
        seastar::seastar_logger.error("error on connection: {}", exc);
        return seastar::make_ready_future<>();
    });
}