#include "Check.hpp"
#include "Loopback.hpp"

TEST_CASE(socketDeliversAcrossThreads)
{
    Endpoint server{};
    UdpSocket a = UdpSocket::bindServer(0, "127.0.0.1");
    UdpSocket b = UdpSocket::bindClient("127.0.0.1", a.localPort(), server);
    std::vector<std::byte> const data(3);
    b.send(server, data);
    std::vector<Datagram> got;
    auto const until = NetClock::now() + std::chrono::seconds(1);
    while (got.empty() && NetClock::now() < until)
        got = a.drain();
    CHECK(got.size() == 1 && got[0].bytes.size() == data.size());
}

TEST_CASE(clientConnectsThenDisconnects)
{
    NetApp server = makeServer();
    NetApp client = connectedClient(server);
    auto &transport = server.scene().getResource<NetTransport>();
    CHECK(transport.peers().size() == 1);
    client.plugin->cleanup(*client.app);
    CHECK(pollUntil({&server}, [&] { return transport.peers().empty(); }));
}
