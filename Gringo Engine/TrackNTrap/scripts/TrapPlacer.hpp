#ifndef TRAPPLACER_HPP_
#define TRAPPLACER_HPP_

#include "Collider3D.hpp"
#include "GameState.hpp"
#include "KeyboardInput.hpp"
#include "PrefabsManager.hpp"
#include "ProjectComponentManager.hpp"
#include "RigidBody3D.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include "Traps.hpp"
#include "UIButton.hpp"
#include "Velocity3D.hpp"
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#define TRAP_DEGREES_TO_RADIANS (3.14159265f / 180.0f)

inline const std::vector<std::string> TrapPlacer_params = {
    "prefab", "count", "distance", "speed", "lifetime", "immunity", "hud", "counter", "key"};
class TrapPlacer : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "prefab")
            _prefab = value;
        else if (name == "count")
            _left = std::stoi(value);
        else if (name == "distance")
            _distance = std::stof(value);
        else if (name == "speed")
            _speed = std::stof(value);
        else if (name == "lifetime")
            _lifetime = std::stof(value);
        else if (name == "immunity")
            _immunity = std::stof(value);
        else if (name == "hud")
            _hudIds = parseIds(value);
        else if (name == "counter")
            _counterId = std::stoi(value);
        else if (name == "key")
            _keyId = std::stoi(value);
    }

    void onStart(Scene &scene, Entity &) override
    {
        EditorEntityIds &ids = scene.getResource<EditorEntityIds>();

        if (!scene.hasResource<Traps>())
            scene.addResource<Traps>();
        for (int id : _hudIds) {
            // No entity with this editor id
            if (ids.entities.count(id) == 0)
                continue;
            Entity entity = ids.entities.at(id);
            if (!scene.hasComponent<UIButton>(entity))
                continue;
            UIButton &button = scene.getComponent<UIButton>(entity);
            _hud.push_back({entity, id, button.width, button.height, button.content});
            hide(button);
        }
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        if (!scene.hasResource<GameState>() || !scene.getResource<GameState>().playing)
            return;
        Traps &traps = scene.getResource<Traps>();

        traps.elapse(FIXED_DELTA_TIME);
        for (Entity done : traps.takeDone())
            scene.queueDestroy(done);
        if (!_hudShown) {
            _hudShown = true;
            for (Panel &panel : _hud)
                show(scene.getComponent<UIButton>(panel.entity), panel);
            updateHud(scene);
        }
        Transform &rat = scene.getComponent<Transform>(entity);
        Collider3D &collider = scene.getComponent<Collider3D>(entity);

        if (scene.getComponent<RigidBody3D>(entity).grounded)
            _floorY = rat.y + collider.offsetY - collider.height / 2.0f;
        bool pressed = scene.getResource<KeyboardInput>().isDown(Key::E);
        if (pressed && !_wasPressed && _left > 0 && scene.getResource<GameState>().canMove)
            place(scene, entity, rat);
        _wasPressed = pressed;
    }

  private:
    struct Panel {
        Entity entity;
        int id;
        int width;
        int height;
        std::string content;
    };

    void place(Scene &scene, Entity rat, Transform &ratTransform)
    {
        if (!scene.getResource<PrefabsManager>().hasPrefab(_prefab))
            return;
        float radians = ratTransform.rotationY * TRAP_DEGREES_TO_RADIANS;
        Entity trap = scene.getResource<PrefabsManager>().InstantiatePrefab(_prefab, scene);

        if (!scene.hasComponent<Transform>(trap))
            scene.addComponent(trap, Transform{});
        Transform &transform = scene.getComponent<Transform>(trap);
        float forwardX = std::sin(radians);
        float forwardZ = -std::cos(radians);

        transform.x += ratTransform.x + forwardX * _distance;
        transform.y += _floorY;
        transform.z += ratTransform.z + forwardZ * _distance;
        transform.rotationY += ratTransform.rotationY - 90.0f;
        Velocity3D velocity;
        velocity.dx = forwardX * _speed;
        velocity.dz = forwardZ * _speed;
        if (scene.hasComponent<Velocity3D>(trap))
            scene.getComponent<Velocity3D>(trap) = velocity;
        else
            scene.addComponent(trap, velocity);
        scene.getResource<Traps>().add(trap, rat, _immunity, _lifetime);
        --_left;
        updateHud(scene);
    }

    void updateHud(Scene &scene)
    {
        for (Panel &panel : _hud) {
            UIButton &button = scene.getComponent<UIButton>(panel.entity);

            if (panel.id == _counterId) {
                button.content = "X" + std::to_string(_left);
            } else if (panel.id == _keyId) {
                if (_left > 0)
                    show(button, panel);
                else
                    hide(button);
            }
        }
    }

    static void hide(UIButton &button)
    {
        button.width = 0;
        button.height = 0;
        button.content.clear();
    }

    static void show(UIButton &button, Panel &panel)
    {
        button.width = panel.width;
        button.height = panel.height;
        if (button.content.empty())
            button.content = panel.content;
    }

    static std::vector<int> parseIds(const std::string &value)
    {
        std::vector<int> ids;
        std::stringstream stream(value);
        std::string id;

        while (std::getline(stream, id, ','))
            if (!id.empty())
                ids.push_back(std::stoi(id));
        return ids;
    }

    std::string _prefab = "MouseTrap";
    int _left = 5;
    float _distance = 12.0f;
    float _speed = 150.0f;
    float _lifetime = 6.0f;
    float _immunity = 5.0f;
    std::vector<int> _hudIds;
    int _counterId = -1;
    int _keyId = -1;
    std::vector<Panel> _hud;
    bool _hudShown = false;
    bool _wasPressed = false;
    float _floorY = 0.0f;
};

#endif /* !TRAPPLACER_HPP_ */
