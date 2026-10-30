/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RTypeClientPlugin
*/

#include "RTypeClientPlugin.hpp"
#include "ClientNetSystem.hpp"
#include "ClientSession.hpp"
#include "NetworkConfig.hpp"
#include "PluginApi.hpp"
#include "SendInputSystem.hpp"
#include "Type.hpp"
#include <string>

void RTypeClientPlugin::build(Application &app)
{
    Scene &scene = app._scene;
    scene.registerComponent<Type>();
    auto const &config = scene.getResource<NetworkConfig>();
    scene.addResource<ClientSession>().server = config.host + ":" + std::to_string(config.port);
    scene.registerSystem<ClientNetSystem>(PrepUpdate);
    scene.registerSystem<SendInputSystem>(Update);
}

void RTypeClientPlugin::cleanup(Application &) {}

PLUGIN_EXPORT IPlugin *createPlugin() { return new RTypeClientPlugin(); }
