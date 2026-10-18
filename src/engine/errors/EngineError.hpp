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

class UnknownComponentType : public std::exception
{
  public:
    UnknownComponentType(const std::string &message)
        : errorMessage("Unknown component type: " + message)
    {
    }

    const char *what() const noexcept override { return errorMessage.c_str(); }

  private:
    std::string errorMessage;
};

class MultipleComponentDefinition : public std::exception
{
  public:
    MultipleComponentDefinition(const std::string &message)
        : errorMessage("Multiple component definition: " + message)
    {
    }

    const char *what() const noexcept override { return errorMessage.c_str(); }

  private:
    std::string errorMessage;
};

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

class ResourceNotFoundError : public EngineError
{
  public:
    ResourceNotFoundError() : EngineError("This resource has not been inserted yet") {}
};

class OutOfRangeEntityError : public EngineError
{
  public:
    OutOfRangeEntityError() : EngineError("Entity out of range") {}
};
