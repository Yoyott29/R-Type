/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RTypeClientPlugin
*/

#include "RTypeClientPlugin.hpp"
#include "ClientNetSystem.hpp"
#include "ClientSession.hpp"
#include "SendInputSystem.hpp"
#include "Type.hpp"
#include <utility>

RTypeClientPlugin::RTypeClientPlugin(std::string serverAddress, std::uint16_t serverPort)
    : _server(std::move(serverAddress) + ":" + std::to_string(serverPort))
{
}

void RTypeClientPlugin::build(Application &app)
{
    Scene &scene = app._scene;
    scene.registerComponent<Type>();
    scene.addResource<ClientSession>().server = _server;
    scene.registerSystem<ClientNetSystem>(PrepUpdate);
    scene.registerSystem<SendInputSystem>(Update);
}

void RTypeClientPlugin::cleanup(Application &) {}
