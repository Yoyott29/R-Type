/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Physics
*/

#include "Physics.hpp"
#include "Collider2D.hpp"
#include "Collider2DSystem.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "MovementSystem.hpp"

void Physics::build(Application &app)
{
    // register the Velocity and Collider2D components
    app._scene.registerComponent<Velocity>();
    app._scene.registerComponent<Collider2D>();
    
    // setup the MovementSystem and its component requirements
    auto movementSystem = app._scene.registerSystem<MovementSystem>(FixedUpdate);
    ComponentSet movementRequirements;
    movementRequirements.set(app._scene.getComponentType<Position>());
    movementRequirements.set(app._scene.getComponentType<Velocity>());
    app._scene.setSystemComponentSet<MovementSystem>(movementRequirements);

    // setup the Collider2DSystem, run after the movement
    app._scene.registerSystem<Collider2DSystem>(FixedUpdate);
    ComponentSet colliderRequirements;
    colliderRequirements.set(app._scene.getComponentType<Position>());
    colliderRequirements.set(app._scene.getComponentType<Collider2D>());
    app._scene.setSystemComponentSet<Collider2DSystem>(colliderRequirements);
}

void Physics::cleanup(Application &app)
{
    (void)app;
    // Cleanup logic for the Physics plugin
}