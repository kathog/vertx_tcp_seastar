//
// Created by nerull on 14.04.2020.
//

#ifndef VERTX_TCP_SEASTAR_HAZELCAST_CLUSTER_H
#define VERTX_TCP_SEASTAR_HAZELCAST_CLUSTER_H
//#include <hazelcast/client/HazelcastAll.h>
#include <hazelcast/client/HazelcastClient.h>
#include <hazelcast/client/MembershipListener.h>
#include <hazelcast/client/InitialMembershipListener.h>
#include <hazelcast/client/InitialMembershipEvent.h>
#include <hazelcast/client/MembershipEvent.h>
#include <hazelcast/client/LifecycleListener.h>
#include <hazelcast/client/serialization/ObjectDataInput.h>
#include <hazelcast/client/serialization/ObjectDataOutput.h>


using haInfo = hazelcast::client::IMap<std::string, std::string>;
using subs = hazelcast::client::MultiMap<std::string, std::string>;

class ServerID {
public:
    ServerID() {};
    ServerID(int port, const std::string &host) : port(port), host(host) {};

private:
    int port;
    std::string host;
};

class ClusterNodeInfo {
public:
    ClusterNodeInfo(const std::string &nodeId, const ServerID &serverId) : nodeId(nodeId), serverId(serverId) {}
    ClusterNodeInfo() {};

private:
    std::string nodeId;
    ServerID serverId;
};

class HaEntryListener: public EntryListener<std::string, std::string> {

public:
    void entryAdded(const EntryEvent<std::string, std::string> &event) override {

        std::cout << "HaEntryListener entryAdded: " <<  event.getKey() << std::endl;
    }

    void entryRemoved(const EntryEvent<std::string, std::string> &event) override {
        std::cout << "HaEntryListener entryRemoved: " <<  event.getKey() << std::endl;
    }

    void entryUpdated(const EntryEvent<std::string, std::string> &event) override {
        std::cout << "HaEntryListener entryUpdated: " <<  event.getKey() << std::endl;
    }

    void entryEvicted(const EntryEvent<std::string, std::string> &event) override {
        std::cout << "HaEntryListener entryEvicted: " <<  event.getKey() << std::endl;
    }

    void entryExpired(const EntryEvent<std::string, std::string> &event) override {
        std::cout << "HaEntryListener entryExpired: " <<  event.getKey() << std::endl;
    }

    void entryMerged(const EntryEvent<std::string, std::string> &event) override {
        std::cout << "HaEntryListener entryMerged: " <<  event.getKey() << std::endl;
    }

    void mapEvicted(const MapEvent &event) override {
        std::cout << "HaEntryListener mapEvicted: " <<  event.getEventType() << std::endl;
    }

    void mapCleared(const MapEvent &event) override {

    }

};


class hazelcast_cluster: public MembershipListener, LifecycleListener, EntryListener<std::string, std::string> {

public:
    hazelcast_cluster(ClientConfig config);

    void join();

    void memberAdded(const MembershipEvent &membershipEvent) override;

    void memberRemoved(const MembershipEvent &membershipEvent) override;

    void memberAttributeChanged(const MemberAttributeEvent &memberAttributeEvent) override;

    virtual ~hazelcast_cluster();

    void addSub (std::string & address, int port, std::string & host);

private:
    void entryAdded(const EntryEvent<std::string, std::string> &event) override;

    void entryRemoved(const EntryEvent<std::string, std::string> &event) override;

    void entryUpdated(const EntryEvent<std::string, std::string> &event) override;

    void entryEvicted(const EntryEvent<std::string, std::string> &event) override;

    void entryExpired(const EntryEvent<std::string, std::string> &event) override;

    void entryMerged(const EntryEvent<std::string, std::string> &event) override;

    void mapEvicted(const MapEvent &event) override;

    void mapCleared(const MapEvent &event) override;

    void stateChanged(const LifecycleEvent &lifecycleEvent) override;

    static constexpr std::string_view _NODE_ID_ATTRIBUTE = "__vertx.nodeId";
    hazelcast::client::ClientConfig _config;
    hazelcast::client::HazelcastClient _hazelcast;
    std::string _nodeID;
    std::string _membershipListenerId;
    std::string _lifecycleListenerId;
    std::shared_ptr<haInfo> _haInfo;
    std::shared_ptr<subs> _subs;
//    SubEntryListener _subListener;
    HaEntryListener _haListener;
};


#endif //VERTX_TCP_SEASTAR_HAZELCAST_CLUSTER_H
