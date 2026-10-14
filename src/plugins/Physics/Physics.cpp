/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Physics
*/

#include "Physics.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "MovementSystem.hpp"

void Physics::build(Application &app)
{
    // register the Velocity component
    app._scene.registerComponent<Velocity>();
    
    // setup the MovementSystem and its component requirements
    auto movementSystem = app._scene.registerSystem<MovementSystem>(FixedUpdate);
    ComponentSet movementRequirements;
    movementRequirements.set(app._scene.getComponentType<Position>());
    movementRequirements.set(app._scene.getComponentType<Velocity>());
    app._scene.setSystemComponentSet<MovementSystem>(movementRequirements);
}

void Physics::cleanup(Application &app)
{
    // Cleanup logic for the Physics plugin
}