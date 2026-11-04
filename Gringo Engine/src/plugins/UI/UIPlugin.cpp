/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** UIPlugin
*/

#include "UIPlugin.hpp"
#include "ButtonClickSystem.hpp"
#include "ButtonClicked.hpp"
#include "ButtonSystem.hpp"
#include "ComponentJson.hpp"
#include "EventBus.hpp"
#include "Position.hpp"
#include "TextSystem.hpp"
#include "UIButton.hpp"
#include "UIText.hpp"
#include <memory>

void UIPlugin::registerComponents(Scene &scene)
{
    // register the UIText component, with how to read it from a .gringo file
    scene.registerComponent<UIText>([](Scene &target, Entity entity, const nlohmann::json &data) {
        target.addComponent(entity,
                            UIText{ComponentJson::value(data, "Content", ""),
                                   ComponentJson::intValue(data, "FontSize", 32),
                                   ComponentJson::colorValue(data, "Color", {255, 255, 255, 255})});
    });
    // register the UIButton component, with how to read it from a .gringo file. Its script is a
    // game class: the generated main creates it from the "Script" property
    scene.registerComponent<UIButton>([](Scene &target, Entity entity, const nlohmann::json &data) {
        target.addComponent(
            entity, UIButton{ComponentJson::value(data, "Content", ""),
                             ComponentJson::intValue(data, "Width", 100),
                             ComponentJson::intValue(data, "Height", 50),
                             ComponentJson::intValue(data, "FontSize", 24),
                             ComponentJson::colorValue(data, "Color", {255, 255, 255, 255}),
                             ComponentJson::colorValue(data, "TextColor", {0, 0, 0, 255}),
                             nullptr});
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

    // setup the ButtonSystem, drawn on top of the world
    app._scene.registerSystem<ButtonSystem>(Overlay);
    ComponentSet buttonRequirements;
    buttonRequirements.set(app._scene.getComponentType<Position>());
    buttonRequirements.set(app._scene.getComponentType<UIButton>());
    app._scene.setSystemComponentSet<ButtonSystem>(buttonRequirements);

    // clicks are emitted as ButtonClicked on the EventBus, then run the button's script
    app._scene.registerSystem<ButtonClickSystem>(PrepUpdate);
    app._scene.setSystemComponentSet<ButtonClickSystem>(buttonRequirements);
    Scene &scene = app._scene;
    _clickSubscription =
        scene.getResource<EventBus>().subscribe<ButtonClicked>([&scene](ButtonClicked const &click) {
            // Kept alive by this copy even if onClick destroys the button
            std::shared_ptr<IGringoButtonScript> script =
                scene.getComponent<UIButton>(click.entity).script;
            if (script)
                script->onClick(scene, click.entity);
        });
}

void UIPlugin::cleanup(Application &app)
{
    app._scene.getResource<EventBus>().unsubscribe<ButtonClicked>(_clickSubscription);
}
