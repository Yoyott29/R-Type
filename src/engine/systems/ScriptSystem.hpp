/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** ScriptSystem
*/

#ifndef SCRIPTSYSTEM_HPP_
#define SCRIPTSYSTEM_HPP_

#include "Scene.hpp"
#include "Script.hpp"

class ScriptSystem : public System
{
  public:
    void Start(Scene &scene);
    void update(Scene &scene);
};

class IGringoEntityScript
{
  public:
    virtual ~IGringoEntityScript() = default;
    virtual void onStart(Scene &scene, Entity &entity) = 0;
    virtual void onUpdate(Scene &scene, Entity &entity) = 0;
};

#endif /* !SCRIPTSYSTEM_HPP_ */
