/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Script
*/

#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_

#include "ComponentDescriptor.hpp"
#include "ComponentPool.hpp"
#include "Entity.hpp"
#include <memory>
#include <string>

class Scene;

class GringoEntityScript
{
  public:
    virtual ~GringoEntityScript() = default;
    virtual void onStart(Scene &scene, Entity &entity) = 0;
    virtual void onUpdate(Scene &scene, Entity &entity) = 0;
    virtual void setParameter(const std::string &, const std::string &) {}
};

struct Script {
    std::shared_ptr<GringoEntityScript> gringoEntityScript;
    bool isRunning = false;
};

template <> struct AllowMultiple<Script> : std::true_type {};

template <> struct ComponentDescription<Script> {
    static constexpr bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Script",
                {
                    {"Path", valueType::File, NO_FILE, {"scripts", ".hpp"}},
                },
                {56, 53, 69, 255},
                ""};
    }
};

#endif /* !SCRIPT_HPP_ */
