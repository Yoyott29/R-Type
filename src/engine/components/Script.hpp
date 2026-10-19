/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Script
*/

#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_

#include "ComponentPool.hpp"
#include "Entity.hpp"
#include <memory>

class Scene;

class GringoEntityScript
{
  public:
    virtual ~GringoEntityScript() = default;
    virtual void onStart(Scene &scene, Entity &entity) = 0;
    virtual void onUpdate(Scene &scene, Entity &entity) = 0;
};

struct Script {
    std::shared_ptr<GringoEntityScript> gringoEntityScript;
    bool isRunning = false;
};

template <> struct AllowMultiple<Script> : std::true_type {};

#endif /* !SCRIPT_HPP_ */
