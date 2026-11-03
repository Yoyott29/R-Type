/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** UIPlugin
*/

#include "UIPlugin.hpp"
#include "ComponentJson.hpp"
#include "Position.hpp"
#include "TextSystem.hpp"
#include "UIText.hpp"

void UIPlugin::registerComponents(Scene &scene)
{
    // register the UIText component, with how to read it from a .gringo file
    scene.registerComponent<UIText>([](Scene &target, Entity entity, const nlohmann::json &data) {
        target.addComponent(entity,
                            UIText{ComponentJson::value(data, "Content", ""),
                                   ComponentJson::intValue(data, "FontSize", 32),
                                   ComponentJson::colorValue(data, "Color", {255, 255, 255, 255})});
    });
}

void UIPlugin::build(Application &app)
{
    // setup the TextSystem, drawn on top of the world
    app._scene.registerSystem<TextSystem>(Overlay);
    ComponentSet textRequirements;
    textRequirements.set(app._scene.getComponentType<Position>());
    textRequirements.set(app._scene.getComponentType<UIText>());
    app._scene.setSystemComponentSet<TextSystem>(textRequirements);
}

void UIPlugin::cleanup(Application &app) { (void)app; }
