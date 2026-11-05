/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Physics2D
*/

#pragma once
#include "Application.hpp"
#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>

class Physics2D : public Plugin<Physics2D>
{
  public:
    Physics2D() = default;
    ~Physics2D() = default;
    void registerComponents(Scene &scene) override;
    void build(Application &app) override;
    void cleanup(Application &app) override;
    // GravityX / GravityY: the Gravity2D resource, in pixels per second squared
    std::vector<PropertyDescriptor> settings() const override;

  private:
    // Reads the Collider2D "TypeCollisions" property: "All layers", "None" or a number (0x6)
    static std::uint32_t maskValue(const nlohmann::json &component);
    // Reads one float project setting of the plugin, throws with its name if invalid
    static float floatSetting(Scene &scene, const std::string &key);
};
