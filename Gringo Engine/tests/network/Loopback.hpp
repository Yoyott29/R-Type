#pragma once

#include "Application.hpp"
#include "Check.hpp"
#include "GameClock.hpp"
#include "NetTransport.hpp"
#include "NetworkConfig.hpp"
#include "NetworkPlugin.hpp"
#include "PrefabsManager.hpp"
#include "UdpSocket.hpp"
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

using Setup = std::function<void(Scene &)>;

// A real Application with the Network plugin, stepped by hand instead of run()
struct NetApp {
    std::unique_ptr<Application> app = std::make_unique<Application>();
    std::shared_ptr<NetworkPlugin> plugin = std::make_shared<NetworkPlugin>();

    Scene &scene() { return app->_scene; }
};

inline NetApp makeNetApp(NetworkConfig config, Setup const &setup = {})
{
    NetApp net;
    net.scene().addResource<PrefabsManager>();
    net.scene().addResource<NetworkConfig>(std::move(config));
    if (setup)
        setup(net.scene());
    net.app->addPlugin(net.plugin);
    return net;
}

inline NetApp makeServer(Setup const &setup = {})
{
    return makeNetApp(NetworkConfig::server(0, "127.0.0.1"), setup);
}

// One frame of every schedule, then one tick
inline void step(NetApp &app)
{
    Scene &scene = app.scene();
    for (auto schedule : {PrepUpdate, StateTransition, FixedUpdate})
        scene.runSchedule(schedule);
    scene.flushDestroyQueue();
    scene.runSchedule(Update);
    scene.runSchedule(PostUpdate);
    ++scene.getResource<GameClock>().tick;
}

// Steps every app until done() holds; false after 1 s
inline bool pollUntil(std::vector<NetApp *> const &apps, std::function<bool()> const &done)
{
    auto const until = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (std::chrono::steady_clock::now() < until) {
        for (NetApp *app : apps)
            step(*app);
        if (done())
            return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return false;
}

// A client of server built with setup, polled until Connected
inline NetApp connectedClient(NetApp &server, Setup const &setup = {})
{
    NetworkConfig config = NetworkConfig::client("127.0.0.1", 0);
    config.port = server.scene().getResource<UdpSocket>().localPort();
    NetApp client = makeNetApp(std::move(config), setup);
    auto &transport = client.scene().getResource<NetTransport>();
    CHECK(pollUntil({&server, &client},
                    [&] { return transport.state() == ConnectionState::Connected; }));
    return client;
}
