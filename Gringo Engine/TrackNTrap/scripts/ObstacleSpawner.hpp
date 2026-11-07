#ifndef OBSTACLESPAWNER_HPP_
#define OBSTACLESPAWNER_HPP_

#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"
#include <array>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#define OBSTACLE_KNIFE_PREFAB "Knife"
#define OBSTACLE_SPATULA_PREFAB "Spatula"
#define OBSTACLE_KNIFE_HEIGHT -1.9f
#define OBSTACLE_SPATULA_SIDE 11.5f
#define OBSTACLE_DEGREES_TO_RADIANS (3.14159265f / 180.0f)

// "KnifeChance": chance for each spot to get a knife instead of a spatula, from 0 to 1
inline const std::vector<std::string> ObstacleSpawner_params = {"KnifeChance"};

// On start, puts an obstacle on each spot of the lap: a knife or a spatula, picked at random, so
// every game has a different mix at the same places
class ObstacleSpawner : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "KnifeChance")
            _knifeChance = std::stof(value);
    }

    void onStart(Scene &scene, Entity &) override
    {
        std::mt19937 generator(std::random_device{}());
        std::bernoulli_distribution knife(_knifeChance);
        std::bernoulli_distribution flipped(0.5);

        for (const Spot &spot : SPOTS) {
            if (knife(generator))
                spawnKnife(scene, spot);
            else
                spawnSpatula(scene, spot, flipped(generator));
        }
    }

    void onUpdate(Scene &, Entity &) override {}

  private:
    // Middle of the lane on the road, and the yaw of the direction across it: (cos, sin) on (X, Z)
    struct Spot {
        float x;
        float y;
        float z;
        float across;
    };

    // Every ~108 units along the lap, from just after the start line to just before the finish
    static constexpr std::array<Spot, 20> SPOTS = {{
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
    }};

    // Slides across the lane: KnifeSpin moves along (sin, cos) of its MoveAngle
    static void spawnKnife(Scene &scene, const Spot &spot)
    {
        const Entity copy = spawn(scene, OBSTACLE_KNIFE_PREFAB, spot.x,
                                  spot.y + OBSTACLE_KNIFE_HEIGHT, spot.z, 0.0f);

        if (scene.hasComponent<Script>(copy))
            scene.getComponent<Script>(copy).gringoEntityScript->setParameter(
                "MoveAngle", std::to_string(90.0f - spot.across));
    }

    // Stands on one side of the lane and slaps towards the other: SpatulaSlap falls towards
    // (cos, sin) of its RotationY
    static void spawnSpatula(Scene &scene, const Spot &spot, bool flipped)
    {
        const float yaw = flipped ? spot.across + 180.0f : spot.across;
        const float acrossX = std::cos(yaw * OBSTACLE_DEGREES_TO_RADIANS);
        const float acrossZ = std::sin(yaw * OBSTACLE_DEGREES_TO_RADIANS);

        spawn(scene, OBSTACLE_SPATULA_PREFAB, spot.x - acrossX * OBSTACLE_SPATULA_SIDE, spot.y,
              spot.z - acrossZ * OBSTACLE_SPATULA_SIDE, yaw);
    }

    // Same placing as a PrefabInstance: the prefab's own Transform is moved by the spot
    static Entity spawn(Scene &scene, const std::string &prefab, float x, float y, float z,
                        float yaw)
    {
        const Entity copy = scene.getResource<PrefabsManager>().InstantiatePrefab(prefab, scene);

        if (!scene.hasComponent<Transform>(copy))
            scene.addComponent(copy, Transform{});
        Transform &transform = scene.getComponent<Transform>(copy);
        transform.x += x;
        transform.y += y;
        transform.z += z;
        transform.rotationY += yaw;
        return copy;
    }

    float _knifeChance = 0.5f;
};

#endif /* !OBSTACLESPAWNER_HPP_ */
