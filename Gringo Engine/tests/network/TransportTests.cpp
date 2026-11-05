#include "Check.hpp"
#include "Events.hpp"
#include "LocalPeer.hpp"
#include "Loopback.hpp"
#include "Packet.hpp"
#include "ReservedInbox.hpp"

TEST_CASE(localPeerFollowsConnection)
{
    NetApp server = makeServer();
    NetApp client = connectedClient(server);
    LocalPeer const &local = client.scene().getResource<LocalPeer>();
    CHECK(local.id == server.scene().getResource<NetTransport>().peers().at(0));
    server.plugin->cleanup(*server.app);
    CHECK(pollUntil({&client}, [&] { return local.id == NO_PEER; }));
}

TEST_CASE(reservedStayOutOfNetMessage)
{
    constexpr std::uint16_t GAME_TYPE = 1;
    auto const ENTITY_STATE = static_cast<std::uint16_t>(ReservedType::EntityState);
    NetApp server = makeServer();
    NetApp client = connectedClient(server);
    auto &transport = server.scene().getResource<NetTransport>();
    transport.broadcast(GAME_TYPE, {});
    transport.broadcastReserved(ReservedType::EntityState, {});

    EventReader<NetMessage> reader;
    bool game = false;
    bool leaked = false;
    bool inboxed = false;
    CHECK(pollUntil({&server, &client}, [&] {
        reader.read(client.scene(), [&](NetMessage const &message) {
            game = game || message.type == GAME_TYPE;
            leaked = leaked || message.type >= RESERVED_TYPE_MIN;
        });
        for (NetMessage const &message : client.scene().getResource<ReservedInbox>().messages)
            inboxed = inboxed || message.type == ENTITY_STATE;
        return game && inboxed;
    }));
    CHECK(!leaked);
}
