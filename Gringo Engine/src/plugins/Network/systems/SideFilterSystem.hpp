/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** SideFilterSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

// Keeps each side's entities: the server destroys Networked{Client}, the client destroys
// Networked{Server|Replicated} that have no Mirror
class SideFilterSystem : public System
{
  public:
    void update(Scene &scene) override;
};
