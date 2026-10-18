/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ResourceManager
*/

#pragma once
#include "EngineError.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>

// Une ressource est une donnee unique par scene (renderer, input, temps...), sans entite
// proprietaire
class IResourceHolder
{
  public:
    virtual ~IResourceHolder() = default;
};

template <typename Resource> class ResourceHolder : public IResourceHolder
{
  public:
    template <typename... Args>
    explicit ResourceHolder(Args &&...args) : value(std::forward<Args>(args)...)
    {
    }

    Resource value;
};

class ResourceManager
{
  public:
    template <typename Resource, typename... Args> Resource &addResource(Args &&...args)
    {
        auto holder = std::make_unique<ResourceHolder<Resource>>(std::forward<Args>(args)...);
        Resource &resource = holder->value;

        _resources.insert_or_assign(std::type_index(typeid(Resource)), std::move(holder));
        return resource;
    }

    template <typename Resource> Resource &getResource()
    {
        auto iterator = _resources.find(std::type_index(typeid(Resource)));

        if (iterator == _resources.end())
            throw ResourceNotFoundError();
        return static_cast<ResourceHolder<Resource> &>(*iterator->second).value;
    }

    template <typename Resource> bool hasResource() const
    {
        return _resources.find(std::type_index(typeid(Resource))) != _resources.end();
    }

    template <typename Resource> void removeResource()
    {
        _resources.erase(std::type_index(typeid(Resource)));
    }

  private:
    std::unordered_map<std::type_index, std::unique_ptr<IResourceHolder>> _resources = {};
};
