/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SystemAccessTests
*/

#include "Check.hpp"
#include "Scene.hpp"
#include "SystemAccess.hpp"
#include <typeindex>

namespace
{
struct Position {
};
struct Speed {
};
struct Clock {
};
struct NoopSystem : System {
    void update(Scene &) override {}
};
} // namespace

TEST_CASE(writeImpliesRead)
{
    Scene scene;
    scene.registerComponent<Position>();
    const SystemAccess access = scene.access().write<Position>();
    const ComponentType type = scene.getComponentType<Position>();
    CHECK(access.componentReads().test(type));
    CHECK(access.componentWrites().test(type));
}

TEST_CASE(readDoesNotWrite)
{
    Scene scene;
    scene.registerComponent<Speed>();
    const SystemAccess access = scene.access().read<Speed>();
    const ComponentType type = scene.getComponentType<Speed>();
    CHECK(access.componentReads().test(type));
    CHECK(!access.componentWrites().test(type));
}

TEST_CASE(resourceWriteImpliesRead)
{
    Scene scene;
    const SystemAccess access = scene.access().writeResource<Clock>();
    const std::type_index key(typeid(Clock));
    CHECK(access.resourceReads().count(key) == 1);
    CHECK(access.resourceWrites().count(key) == 1);
}

TEST_CASE(unregisteredComponentThrows)
{
    Scene scene;
    CHECK_THROWS(scene.access().read<Position>(), ComponentNotRegisteredError);
}

TEST_CASE(setAccessOnUnknownSystemThrows)
{
    Scene scene;
    CHECK_THROWS(scene.setSystemAccess<NoopSystem>(scene.access()), ComponentNotRegisteredError);
}
