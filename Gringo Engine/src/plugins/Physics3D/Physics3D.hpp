/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Physics3D
*/

#pragma once
#include "Application.hpp"
#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>

class Physics3D : public Plugin<Physics3D> {
  public:
    void registerComponents(Scene &scene) override;
    void build(Application &app) override;
    void cleanup(Application &app) override;
    std::vector<PropertyDescriptor> settings() const override;

  private:
    static std::uint32_t maskValue(const nlohmann::json &component);
    static float floatSetting(Scene &scene, const std::string &key);
};
