/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** EngineErrors
*/

#pragma once
#include "Entity.hpp"
#include <stdexcept>
#include <string>

class EngineError : public std::runtime_error
{
  public:
    EngineError(std::string const &message) : std::runtime_error(message) {}
};

class MissingComponentError : public EngineError
{
  public:
    MissingComponentError(Entity entity)
        : EngineError("Entity " + std::to_string(entity) + " does not have the requested component")
    {
    }
};

class EntityLimitError : public EngineError
{
  public:
    EntityLimitError() : EngineError("Too many entities alive at once") {}
};

class RegisterError : public EngineError
{
  public:
    RegisterError() : EngineError("This component/system has already been registered") {}
};

class ComponentNotRegisteredError : public EngineError
{
  public:
    ComponentNotRegisteredError() : EngineError("This component is not registered yet") {}
};

class OutOfRangeEntityError : public EngineError
{
  public:
    OutOfRangeEntityError() : EngineError("Entity out of range") {}
};
