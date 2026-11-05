/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** SideFilterSystem
*/

#include "SideFilterSystem.hpp"
#include "NetComponents.hpp"
#include "NetworkConfig.hpp"
#include "Networked.hpp"
#include <vector>

void SideFilterSystem::update(Scene &scene)
{
    bool const server = scene.getResource<NetworkConfig>().role == NetRole::Server;
    // Collected first: destroying also takes the ChildOf descendants
    std::vector<Entity> doomed;
    for (Entity entity : scene.view<Networked>()) {
        NetMode const mode = scene.getComponent<Networked>(entity).mode;
        bool const remove = server ? mode == NetMode::Client
                                   : mode != NetMode::Client && !scene.hasComponent<Mirror>(entity);
        if (remove)
            doomed.push_back(entity);
    }
    for (Entity entity : doomed)
        scene.destroyEntity(entity);
}
