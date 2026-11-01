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
    void Start(Scene &scene) override;
    void update(Scene &scene);
};

#endif /* !SCRIPTSYSTEM_HPP_ */
