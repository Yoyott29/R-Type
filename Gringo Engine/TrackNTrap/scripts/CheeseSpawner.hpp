#ifndef CHEESESPAWNER_HPP_
#define CHEESESPAWNER_HPP_

#include "Cheeses.hpp"
#include "PrefabsManager.hpp"
#include "Net.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

#define CHEESE_DEGREES_TO_RADIANS (3.14159265f / 180.0f)
#define CHEESE_TWO_PI 6.2831853f
#define CHEESE_SIDE_RANGE 8.0f

inline const std::vector<std::string> CheeseSpawner_params = {
    "prefab", "count", "respawn", "height", "bobHeight", "bobSpeed", "spinSpeed"};

class CheeseSpawner : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "prefab")
            _prefab = value;
        else if (name == "count")
            _count = std::stoi(value);
        else if (name == "respawn")
            _respawn = std::stof(value);
        else if (name == "height")
            _height = std::stof(value);
        else if (name == "bobHeight")
            _bobHeight = std::stof(value);
        else if (name == "bobSpeed")
            _bobSpeed = std::stof(value);
        else if (name == "spinSpeed")
            _spinSpeed = std::stof(value);
    }

    void onStart(Scene &scene, Entity &) override
    {
        // Server only: the clients get the cheeses it puts
        if (Net::isClient(scene))
            return;
        if (!scene.hasResource<Cheeses>())
            scene.addResource<Cheeses>();
        for (int i = 0; i < _count; ++i)
            spawn(scene);
    }

    void onUpdate(Scene &scene, Entity &) override
    {
        // Server only: the clients get the cheeses it puts
        if (Net::isClient(scene))
            return;
        Cheeses &cheeses = scene.getResource<Cheeses>();

        for (Entity eaten : cheeses.takeEaten()) {
            for (int i = 0; i < static_cast<int>(_floating.size()); ++i) {
                if (_floating.at(i).entity == eaten) {
                    _floating.erase(_floating.begin() + i);
                    break;
                }
            }
            scene.queueDestroy(eaten);
            _waiting.push_back(_respawn);
        }

        int i = 0;
        while (i < static_cast<int>(_waiting.size())) {
            _waiting.at(i) -= FIXED_DELTA_TIME;
            if (_waiting.at(i) <= 0.0f) {
                _waiting.erase(_waiting.begin() + i);
                spawn(scene);
            } else {
                ++i;
            }
        }

        _time += FIXED_DELTA_TIME;

        for (Floating &cheese : _floating) {
            Transform &transform = scene.getComponent<Transform>(cheese.entity);

            transform.y = cheese.baseY +
                          _bobHeight * std::sin(CHEESE_TWO_PI * (_bobSpeed * _time + cheese.phase));
            transform.rotationY =
                std::fmod(transform.rotationY + _spinSpeed * FIXED_DELTA_TIME, 360.0f);
        }
    }

  private:
    struct Spot {
        float x;
        float y;
        float z;
        float across;
    };

    struct Floating {
        Entity entity;
        int spot;
        float baseY;
        float phase;
    };

    inline static const std::vector<Spot> SPOTS = {
        {-72.9f, -0.072f, 31.4f, -91.3f},   {-19.0f, -0.538f, 30.2f, -91.3f},
        {34.9f, -0.990f, 29.0f, -91.3f},    {88.8f, -1.378f, 27.8f, -91.3f},
        {142.8f, -1.844f, 26.6f, -91.3f},   {196.5f, -2.148f, 28.4f, -78.7f},
        {246.2f, -2.148f, 48.6f, -57.3f},   {285.1f, -2.148f, 85.5f, -35.9f},
        {307.8f, -2.148f, 134.0f, -14.4f},  {311.3f, -2.148f, 187.5f, 7.0f},
        {308.5f, -2.148f, 241.2f, 7.1f},    {294.1f, -2.148f, 292.9f, 23.7f},
        {265.6f, -1.840f, 338.5f, 40.3f},   {225.2f, -1.542f, 373.9f, 57.0f},
        {176.4f, -1.238f, 396.4f, 73.6f},   {123.1f, -0.959f, 403.9f, 90.2f},
        {70.3f, -0.652f, 405.0f, 91.0f},    {16.4f, -0.498f, 404.1f, 91.0f},
        {-37.5f, -0.622f, 403.2f, 91.0f},   {-91.4f, -0.886f, 402.2f, 91.0f},
        {-145.3f, -1.145f, 401.3f, 91.0f},  {-199.3f, -0.949f, 400.4f, 91.0f},
        {-253.2f, -0.778f, 399.5f, 91.0f},  {-307.1f, -0.605f, 398.6f, 91.0f},
        {-360.9f, -0.636f, 399.5f, 93.7f},  {-413.7f, -0.474f, 389.1f, 108.5f},
        {-462.0f, -0.312f, 365.5f, 123.6f}, {-502.5f, -0.146f, 330.1f, 138.7f},
        {-532.5f, 0.013f, 285.5f, 153.5f},  {-549.9f, 0.096f, 234.6f, 168.6f},
        {-553.5f, 0.172f, 180.9f, -176.4f}, {-541.6f, 0.261f, 129.6f, -159.5f},
        {-513.5f, 0.358f, 83.9f, -137.1f},  {-470.2f, 0.454f, 52.4f, -114.9f},
        {-418.5f, 0.561f, 39.1f, -91.3f},   {-364.6f, 0.602f, 37.9f, -91.3f},
        {-310.7f, 0.602f, 36.7f, -91.3f},   {-256.8f, 0.602f, 35.5f, -91.3f},
    };

    bool taken(int spot)
    {
        for (Floating &cheese : _floating) {
            if (cheese.spot == spot)
                return true;
        }
        return false;
    }

    // A new cheese on a spot no cheese is on, somewhere across the lane
    void spawn(Scene &scene)
    {
        std::vector<int> free;

        for (int spot = 0; spot < static_cast<int>(SPOTS.size()); ++spot) {
            if (!taken(spot))
                free.push_back(spot);
        }
        if (free.empty() || !scene.getResource<PrefabsManager>().hasPrefab(_prefab))
            return;

        int index = free.at(std::rand() % static_cast<int>(free.size()));
        Spot spot = SPOTS.at(index);
        // Between -CHEESE_SIDE_RANGE and CHEESE_SIDE_RANGE
        float side = -CHEESE_SIDE_RANGE + 2.0f * CHEESE_SIDE_RANGE * std::rand() / static_cast<float>(RAND_MAX);
        Entity cheese = scene.getResource<PrefabsManager>().InstantiatePrefab(_prefab, scene);

        if (!scene.hasComponent<Transform>(cheese))
            scene.addComponent(cheese, Transform{});
        Transform &transform = scene.getComponent<Transform>(cheese);
        transform.x += spot.x + std::cos(spot.across * CHEESE_DEGREES_TO_RADIANS) * side;
        transform.z += spot.z + std::sin(spot.across * CHEESE_DEGREES_TO_RADIANS) * side;
        transform.rotationY += 360.0f * std::rand() / static_cast<float>(RAND_MAX);
        _floating.push_back(
            {cheese, index, transform.y + spot.y + _height, std::rand() / static_cast<float>(RAND_MAX)});
        scene.getResource<Cheeses>().add(cheese);
    }

    std::string _prefab = "Cheese";
    int _count = 4;
    float _respawn = 5.0f;
    float _height = 2.0f;
    float _bobHeight = 0.5f;
    float _bobSpeed = 0.5f;
    float _spinSpeed = 90.0f;
    float _time = 0.0f;
    std::vector<Floating> _floating;
    std::vector<float> _waiting;
};

#endif /* !CHEESESPAWNER_HPP_ */
