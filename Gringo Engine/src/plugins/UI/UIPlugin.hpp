/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** UIPlugin
*/

#pragma once
#include "Application.hpp"
#include <cstddef>

class UIPlugin : public Plugin<UIPlugin>
{
  public:
    void registerComponents(Scene &scene) override;
    void build(Application &app) override;
    void cleanup(Application &app) override;
    bool requiresRenderer() const override { return true; }

  private:
    // Listens to ButtonClicked on the EventBus and calls the clicked button's script
    std::size_t _clickSubscription = 0;
};
