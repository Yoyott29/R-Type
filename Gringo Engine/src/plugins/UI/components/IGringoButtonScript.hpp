/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** IGringoButtonScript
*/

#pragma once
#include "Entity.hpp"

class Scene;

// Script of a UIButton (its "Script" property), different from the entity scripts: it only
// reacts to clicks. A game script: class PlayButton : public IGringoButtonScript
class IGringoButtonScript
{
  public:
    virtual ~IGringoButtonScript() = default;
    // Called when the button is clicked, with the button's entity
    virtual void onClick(Scene &scene, Entity entity) = 0;
};
