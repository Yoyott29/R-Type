/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Physics
*/

#pragma once
#include "Application.hpp"
#include <cstdint>
#include <nlohmann/json_fwd.hpp>

class Physics : public IPlugin
{
  public:
    Physics() = default;
    ~Physics() = default;
    void registerComponents(Scene &scene) override;
    void build(Application &app) override;
    void cleanup(Application &app) override;

  private:
    // Reads the Collider2D "TypeCollisions" property: "All layers", "None" or a number (0x6)
    static std::uint32_t maskValue(const nlohmann::json &component);
};
