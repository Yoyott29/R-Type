#include "ComponentManager.hpp"
#include "EngineError.hpp"
#include "nlohmann/json.hpp"

void ComponentManager::addComponentFromJson(Scene &scene, Entity entity,
                                            const nlohmann::json &component)
{
    const std::string name = component.value("name", "");
    auto iterator = _deserializers.find(name);
    if (iterator == _deserializers.end())
        throw UnknownComponentType(name);
    iterator->second(scene, entity, component);
}

std::function<void(Scene &, Entity)>
ComponentManager::componentInitializer(const nlohmann::json &component) const
{
    const std::string name = component.value("name", "");
    auto iterator = _deserializers.find(name);
    if (iterator == _deserializers.end())
        throw UnknownComponentType(name);

    const ComponentDeserializer deserializer = iterator->second;
    return [deserializer, component](Scene &scene, Entity entity) {
        deserializer(scene, entity, component);
    };
}
