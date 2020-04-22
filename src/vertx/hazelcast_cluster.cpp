//
// Created by nerull on 14.04.2020.
//

#include "vertx/hazelcast_cluster.h"
#include <seastar/core/reactor.hh>
#include <memory>

using namespace std::chrono_literals;


hazelcast_cluster::hazelcast_cluster(hazelcast::client::ClientConfig config) : _config(std::move(config)), _hazelcast(_config) {

}
//__vertx.haInfo
//__vertx.subs
void hazelcast_cluster::join() {
    _membershipListenerId = _hazelcast.getCluster().addMembershipListener(boost::shared_ptr<hazelcast_cluster>(this));
    _hazelcast.getLifecycleService().addLifecycleListener(this);
    _nodeID = *_hazelcast.getLocalEndpoint().getUuid();

    seastar::seastar_logger.debug("add cluster memberID: {}", _nodeID);

    seastar::seastar_logger.debug("__vertx.haInfo members:");
    _haInfo = std::make_shared<haInfo>(_hazelcast.getMap<std::string, std::string>("__vertx.haInfo"));
    for (auto & [key, value] : _haInfo->entrySet()) {
        seastar::seastar_logger.debug("memberID: {}, member: {}", key, value);
    }

    seastar::seastar_logger.debug("__vertx.subs members:");
    _subs = std::make_shared<subs> (_hazelcast.getMultiMap<std::string, std::string>("__vertx.subs"));
    for (auto & [key, value] : _subs->entrySet()) {
        seastar::seastar_logger.debug("memberID: {}, member: {}", key, value);
    }

    _subs->addEntryListener(*this, true);
    _haListener = {};
    _haInfo->addEntryListener(_haListener, true);

    _haInfo->put(_nodeID, "{\"verticles\":[],\"group\":\"__DISABLED__\",\"server_id\":{\"host\":\"localhost\",\"port\":10000}}");

    std::this_thread::sleep_for(1s);
}

void hazelcast_cluster::addSub (std::string & address, int port, std::string & host) {
    _subs->put(address, "{\"nodeId\":\""+ _nodeID +"\",\"serverID\":{\"port\":" + std::to_string(port) + ",\"host\":\""+host+"\"}}");
}

void hazelcast_cluster::memberAdded(const MembershipEvent &membershipEvent) {
    std::cout << "MembershipEvent memberAdded: " << membershipEvent.getMember().getUuid() << std::endl;
}

void hazelcast_cluster::memberRemoved(const MembershipEvent &membershipEvent) {
    std::cout << "MembershipEvent memberRemoved: " << membershipEvent.getMember().getUuid() << std::endl;
}

void hazelcast_cluster::memberAttributeChanged(const MemberAttributeEvent &memberAttributeEvent) {

}

void hazelcast_cluster::stateChanged(const LifecycleEvent &lifecycleEvent) {
    std::cout << "stateChanged: " << lifecycleEvent.getState() << std::endl;
}

void hazelcast_cluster::entryAdded(const EntryEvent<std::string, std::string> &event) {
    std::cout << "SubEntryListener entryAdded: " << event.getKey() << std::endl;
}

void hazelcast_cluster::entryRemoved(const EntryEvent<std::string, std::string> &event) {
    std::cout << "SubEntryListener entryAdded: " << event.getKey() << std::endl;
}

void hazelcast_cluster::entryUpdated(const EntryEvent<std::string, std::string> &event) {

}

void hazelcast_cluster::entryEvicted(const EntryEvent<std::string, std::string> &event) {

}

void hazelcast_cluster::entryExpired(const EntryEvent<std::string, std::string> &event) {

}

void hazelcast_cluster::entryMerged(const EntryEvent<std::string, std::string> &event) {

}

void hazelcast_cluster::mapEvicted(const MapEvent &event) {

}

void hazelcast_cluster::mapCleared(const MapEvent &event) {

}

hazelcast_cluster::~hazelcast_cluster() {
}
