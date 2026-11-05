#include "Check.hpp"
#include "PrefabsManager.hpp"
#include "ProjectComponentManager.hpp"

TEST_CASE(hasPrefab)
{
    PrefabsManager prefabs;
    prefabs.createPrefab("Player", Prefab{});
    CHECK(prefabs.hasPrefab("Player") && !prefabs.hasPrefab("Ghost"));
}

TEST_CASE(ignoredComponentIsSkipped)
{
    Scene scene;
    ProjectComponentManager components(scene, ".");
    const nlohmann::json sprite = {{"name", "Sprite"}, {"properties", nlohmann::json::array()}};
    CHECK_THROWS(components.addToScene(scene, scene.createEntity(), sprite), UnknownComponentType);
    components.ignore({"Sprite"});
    components.addToScene(scene, scene.createEntity(), sprite);
}
