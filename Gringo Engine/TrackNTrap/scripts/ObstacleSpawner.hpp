#ifndef OBSTACLESPAWNER_HPP_
#define OBSTACLESPAWNER_HPP_

#include "PrefabsManager.hpp"
#include "Net.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

#define OBSTACLE_KNIFE_PREFAB "Knife"
#define OBSTACLE_SPATULA_PREFAB "Spatula"
#define OBSTACLE_BROOM_PREFAB "Broom"
#define OBSTACLE_KNIFE_HEIGHT -1.9f
#define OBSTACLE_SPATULA_SIDE 11.5f
#define OBSTACLE_DEGREES_TO_RADIANS (3.14159265f / 180.0f)

inline const std::vector<std::string> ObstacleSpawner_params = {"KnifeWeight", "SpatulaWeight",
                                                                "BroomWeight"};

class ObstacleSpawner : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "KnifeWeight")
            _knifeWeight = std::stof(value);
        else if (name == "SpatulaWeight")
            _spatulaWeight = std::stof(value);
        else if (name == "BroomWeight")
            _broomWeight = std::stof(value);
    }

    void onStart(Scene &scene, Entity &) override
    {
        // Server only: the clients get the obstacles it puts
        if (Net::isClient(scene))
            return;
        float total = _knifeWeight + _spatulaWeight + _broomWeight;

        for (Spot spot : SPOTS) {
            float pick = total * std::rand() / static_cast<float>(RAND_MAX);

            if (pick < _knifeWeight)
                spawnKnife(scene, spot);
            else if (pick < _knifeWeight + _spatulaWeight)
                spawnSpatula(scene, spot, std::rand() % 2 == 0);
            else
                spawn(scene, OBSTACLE_BROOM_PREFAB, spot.x, spot.y, spot.z, spot.across);
        }
    }

    void onUpdate(Scene &, Entity &) override {}

  private:
    struct Spot {
        float x;
        float y;
        float z;
        float across;
    };

    inline static const std::vector<Spot> SPOTS = {
        {-99.9f, 0.161f, 32.0f, -91.3f},    {8.0f, -0.771f, 29.6f, -91.3f},
        {115.8f, -1.604f, 27.2f, -91.3f},   {222.3f, -2.148f, 36.2f, -67.9f},
        {298.7f, -2.148f, 108.7f, -25.0f},  {309.9f, -2.148f, 214.2f, -1.3f},
        {281.5f, -2.031f, 316.8f, 32.0f},   {201.6f, -1.374f, 386.9f, 65.3f},
        {97.3f, -0.836f, 405.4f, 91.0f},    {-10.5f, -0.5f, 403.6f, 91.0f},
        {-118.4f, -0.996f, 401.8f, 91.0f},  {-226.2f, -0.862f, 400.0f, 91.0f},
        {-334.0f, -0.578f, 399.4f, 86.2f},  {-438.6f, -0.392f, 378.9f, 116.1f},
        {-518.9f, -0.066f, 308.8f, 146.2f}, {-553.4f, 0.134f, 207.9f, 176.1f},
        {-529.8f, 0.308f, 105.4f, -148.3f}, {-444.8f, 0.502f, 43.5f, -103.7f},
        {-337.7f, 0.602f, 37.3f, -91.3f},   {-229.8f, 0.602f, 34.9f, -91.3f},
    };

    static void spawnKnife(Scene &scene, Spot spot)
    {
        Entity copy = spawn(scene, OBSTACLE_KNIFE_PREFAB, spot.x, spot.y + OBSTACLE_KNIFE_HEIGHT,
                            spot.z, 0.0f);

        if (scene.hasComponent<Script>(copy)) {
            Script &script = scene.getComponent<Script>(copy);
            script.gringoEntityScript->setParameter("MoveAngle",
                                                    std::to_string(90.0f - spot.across));
        }
    }

    static void spawnSpatula(Scene &scene, Spot spot, bool flipped)
    {
        float yaw = spot.across;
        if (flipped)
            yaw += 180.0f;
        float acrossX = std::cos(yaw * OBSTACLE_DEGREES_TO_RADIANS);
        float acrossZ = std::sin(yaw * OBSTACLE_DEGREES_TO_RADIANS);

        spawn(scene, OBSTACLE_SPATULA_PREFAB, spot.x - acrossX * OBSTACLE_SPATULA_SIDE, spot.y,
              spot.z - acrossZ * OBSTACLE_SPATULA_SIDE, yaw);
    }

    static Entity spawn(Scene &scene, const std::string &prefab, float x, float y, float z,
                        float yaw)
    {
        Entity copy = scene.getResource<PrefabsManager>().InstantiatePrefab(prefab, scene);

        if (!scene.hasComponent<Transform>(copy))
            scene.addComponent(copy, Transform{});
        Transform &transform = scene.getComponent<Transform>(copy);
        transform.x += x;
        transform.y += y;
        transform.z += z;
        transform.rotationY += yaw;
        return copy;
    }

    float _knifeWeight = 1.0f;
    float _spatulaWeight = 1.0f;
    float _broomWeight = 1.0f;
};

#endif /* !OBSTACLESPAWNER_HPP_ */
