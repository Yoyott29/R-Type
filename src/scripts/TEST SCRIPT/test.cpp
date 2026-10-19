/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** test
*/

#include "test.hpp"
#include <iostream>

void Test::onStart(Scene &scene, Entity &entity)
{
    std::cout << "Test script started for entity " << entity << std::endl;
}

void Test::onUpdate(Scene &scene, Entity &entity)
{
    std::cout << "Test script updated for entity " << entity << std::endl;
}
