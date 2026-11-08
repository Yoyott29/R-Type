/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** UIPlugin
*/

#include "UIPlugin.hpp"
#include "ButtonClickSystem.hpp"
#include "ButtonSystem.hpp"
#include "ComponentJson.hpp"
#include "ProjectAssets.hpp"
#include "TextSystem.hpp"
#include "Transform.hpp"
#include "UIButton.hpp"
#include "UIText.hpp"
#include <memory>

// The "Font" property, resolved in the project's assets; no file chosen: the built-in font
static std::string fontPath(Scene &scene, const nlohmann::json &data)
{
    const std::string font = ComponentJson::value(data, "Font", DEFAULT_FONT);
    if (font.empty() || font == NO_FILE)
        return "";
    return ProjectAssets::resolve(scene, font);
}

// The "ImagePath" property, resolved in the project's assets; no file chosen: no image
static std::string imagePath(Scene &scene, const nlohmann::json &data)
{
    const std::string image = ComponentJson::value(data, "ImagePath", NO_FILE);
    if (image.empty() || image == NO_FILE)
        return "";
    return ProjectAssets::resolve(scene, image);
}

void UIPlugin::registerComponents(Scene &scene)
{
    // register the UIText component, with how to read it from a .gringo file
    scene.registerComponent<UIText>([](Scene &target, Entity entity, const nlohmann::json &data) {
        target.addComponent(entity,
                            UIText{ComponentJson::value(data, "Content", ""),
                                   ComponentJson::intValue(data, "FontSize", 32),
                                   ComponentJson::colorValue(data, "Color", {255, 255, 255, 255}),
                                   fontPath(target, data)});
    });
    // register the UIButton component, with how to read it from a .gringo file. Its script is a
    // game class: the generated main creates it from the "Script" property
    scene.registerComponent<UIButton>([](Scene &target, Entity entity, const nlohmann::json &data) {
        target.addComponent(entity,
                            UIButton{ComponentJson::value(data, "Content", ""),
                                     ComponentJson::intValue(data, "Width", 100),
                                     ComponentJson::intValue(data, "Height", 50),
                                     ComponentJson::intValue(data, "FontSize", 24),
                                     ComponentJson::colorValue(data, "Color", {255, 255, 255, 255}),
                                     ComponentJson::colorValue(data, "TextColor", {0, 0, 0, 255}),
                                     {},
                                     fontPath(target, data),
                                     imagePath(target, data)});
    });
}

void UIPlugin::build(Application &app)
{
    // setup the TextSystem, drawn on top of the world
    app._scene.registerSystem<TextSystem>(Overlay);
    ComponentSet textRequirements;
    textRequirements.set(app._scene.getComponentType<Transform>());
    textRequirements.set(app._scene.getComponentType<UIText>());
    app._scene.setSystemComponentSet<TextSystem>(textRequirements);

    // setup the ButtonSystem, drawn on top of the world
    app._scene.registerSystem<ButtonSystem>(Overlay);
    ComponentSet buttonRequirements;
    buttonRequirements.set(app._scene.getComponentType<Transform>());
    buttonRequirements.set(app._scene.getComponentType<UIButton>());
    app._scene.setSystemComponentSet<ButtonSystem>(buttonRequirements);

    // calls the script of the clicked buttons
    app._scene.registerSystem<ButtonClickSystem>(PrepUpdate);
    app._scene.setSystemComponentSet<ButtonClickSystem>(buttonRequirements);
}

void UIPlugin::cleanup(Application &app) { (void)app; }
