/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetworkPlugin
*/

#pragma once
#include "Application.hpp"
#include "NetworkConfig.hpp"
#include "Scene.hpp"
#include <cstdint>
#include <string>
#include <vector>

// Transport-only UDP networking: adds the socket, NetTransport and the event queues to a scene.
// Uses the NetworkConfig resource if the executable added one, otherwise builds it from the
// project settings (Host, Port), overridden by the command line: "[host] port". The role comes
// from the build variant: "server" or "client"
class NetworkPlugin : public Plugin<NetworkPlugin>
{
  public:
    void build(Application &app) override;
    void cleanup(Application &app) override;
    std::vector<PropertyDescriptor> settings() const override;
    std::vector<BuildVariant> buildVariants() const override;
    // The NetworkConfig resource if there is one, otherwise built from the settings, the command
    // line and BuildVariantName. Throws EngineError without either, or on an unknown variant
    static NetworkConfig resolveConfig(Scene &scene);

  private:
    void build(Scene &scene);
    void cleanup(Scene &scene);
    // Throws EngineError if the text is not a port number (1-65535)
    static std::uint16_t parsePort(const std::string &text);
};
