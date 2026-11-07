/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** Audio
*/

#include "Audio.hpp"
#include "AudioSource.hpp"
#include "AudioSystem.hpp"
#include "ComponentJson.hpp"
#include "ProjectAssets.hpp"

void Audio::registerComponents(Scene &scene)
{
    scene.registerComponent<AudioSource>([](Scene &target, Entity entity,
                                            const nlohmann::json &data) {
        AudioSource source;
        const std::string path = ComponentJson::value(data, "Path", NO_FILE);
        if (path != NO_FILE)
            source.path = ProjectAssets::resolve(target, path);
        source.volume = ComponentJson::intValue(data, "Volume", 100);
        source.loop = ComponentJson::boolValue(data, "Loop", false);
        if (ComponentJson::boolValue(data, "PlayOnStart", true))
            source.play();
        target.addComponent(entity, source);
    });
}

void Audio::build(Application &app)
{
    app._scene.registerSystem<AudioSystem>(Update);
    ComponentSet audioRequirements;
    audioRequirements.set(app._scene.getComponentType<AudioSource>());
    app._scene.setSystemComponentSet<AudioSystem>(audioRequirements);
}

void Audio::cleanup(Application &app) { (void)app; }
