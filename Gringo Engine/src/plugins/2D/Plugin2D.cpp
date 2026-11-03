/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Plugin2D
*/

#include "Plugin2D.hpp"
#include "ComponentJson.hpp"
#include "Position.hpp"
#include "ProjectAssets.hpp"
#include "SpriteSystem.hpp"
#include "Sprite.hpp"

void Plugin2D::registerComponents(Scene &scene)
{
    // register the Sprite component, with how to read it from a .gringo file
    scene.registerComponent<Sprite>([](Scene &target, Entity entity, const nlohmann::json &data) {
        Sprite sprite;
        if (ComponentJson::value(data, "Shape", "Box") == "Circle")
            sprite.shape = CircleShape{ComponentJson::floatValue(data, "Radius", 16.0f)};
        else
            sprite.shape = BoxShape{ComponentJson::floatValue(data, "Width", 32.0f),
                                    ComponentJson::floatValue(data, "Height", 32.0f)};
        sprite.drawOrder = ComponentJson::intValue(data, "DrawOrder", 0);
        const std::string texture = ComponentJson::value(data, "TexturePath", "");
        // No file chosen: the shape is drawn in its color ("default.png" in older projects)
        const bool noTexture = texture.empty() || texture == NO_FILE || texture == "default.png";
        sprite.texturePath = noTexture ? "" : ProjectAssets::resolve(target, texture);
        sprite.color = ComponentJson::colorValue(data, "Color", {255, 255, 255, 255});
        target.addComponent(entity, sprite);
    });
}

void Plugin2D::build(Application &app)
{
    // setup the SpriteSystem, drawn in the Render schedule
    app._scene.registerSystem<SpriteSystem>(Render);
    ComponentSet spriteRequirements;
    spriteRequirements.set(app._scene.getComponentType<Position>());
    spriteRequirements.set(app._scene.getComponentType<Sprite>());
    app._scene.setSystemComponentSet<SpriteSystem>(spriteRequirements);
}

void Plugin2D::cleanup(Application &app) { (void)app; }
