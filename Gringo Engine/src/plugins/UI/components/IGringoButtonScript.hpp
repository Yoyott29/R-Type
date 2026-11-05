/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** IGringoButtonScript
*/

#pragma once
#include "Entity.hpp"
#include <string>

class Scene;

// Script of a UIButton (its "Script" property), different from the entity scripts: it only
// reacts to clicks. A game script: class PlayButton : public IGringoButtonScript
// Like entity scripts it can declare parameters, edited in the Button's properties:
//   inline const std::vector<std::string> PlayButton_params = {"Target"};
class IGringoButtonScript
{
  public:
    virtual ~IGringoButtonScript() = default;
    // Called when the button is clicked, with the button's entity
    virtual void onClick(Scene &scene, Entity entity) = 0;
    // Called once per parameter before the game starts, with the value set in the editor
    virtual void setParameter(const std::string &name, const std::string &value)
    {
        (void)name;
        (void)value;
    }
};
