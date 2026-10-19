/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** test
*/

#ifndef TEST_HPP_
#define TEST_HPP_

#include "ScriptSystem.hpp"

class Test : public IGringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
};

#endif /* !TEST_HPP_ */
