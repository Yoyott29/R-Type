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
#include "TrapImmunity.hpp"
#include "UIButton.hpp"
#include <algorithm>
#include <cmath>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

// Bounds of MouseTrap.glb as drawn (after the renderer's asset yaw), board bottom at Y 0
#define TRAP_MIN_X -1.031f
#define TRAP_MAX_X 0.747f
#define TRAP_MIN_Y -0.038f
#define TRAP_MAX_Y 0.348f
#define TRAP_MIN_Z -0.749f
#define TRAP_MAX_Z 0.442f
#define TRAP_DEGREES_TO_RADIANS (3.14159265f / 180.0f)

// "prefab": the trap prefab. "count": traps per game. "distance": how far in front of the rat it
// lands. "immunity": seconds the trap can't kill the rat that placed it. "hud": editor ids of the
// HUD buttons, comma separated, hidden on the menu. "counter": the one whose text is "X<traps
// left>". "key": the one showing the key, hidden when no trap is left
inline const std::vector<std::string> TrapPlacer_params = {"prefab", "count", "distance",
                                                           "immunity", "hud", "counter", "key"};

// On the rat: E drops a trap on the floor in front of it, up to "count" per game. The trap is a
// trigger, so any rat running into it dies, the one that placed it too once its immunity is over
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
        const EditorEntityIds &ids = scene.getResource<EditorEntityIds>();

        if (!scene.hasResource<TrapImmunity>())
            scene.addResource<TrapImmunity>();
        for (int id : _hudIds) {
            const std::optional<Entity> entity = ids.find(id);

            if (!entity || !scene.hasComponent<UIButton>(*entity))
                continue;
            UIButton &button = scene.getComponent<UIButton>(*entity);
            _hud.push_back({*entity, id, button.width, button.height, button.content});
            show(button, false);
        }
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        if (!scene.hasResource<GameState>() || !scene.getResource<GameState>().playing)
            return;
        scene.getResource<TrapImmunity>().elapse(FIXED_DELTA_TIME);
        if (!_hudShown) {
            _hudShown = true;
            for (const Panel &panel : _hud)
                show(scene.getComponent<UIButton>(panel.entity), true, panel);
            updateHud(scene);
        }
        const Transform &rat = scene.getComponent<Transform>(entity);
        const Collider3D &collider = scene.getComponent<Collider3D>(entity);

        // Where the feet are when on the floor: a trap dropped mid-jump still lands there
        if (scene.getComponent<RigidBody3D>(entity).grounded)
            _floorY = rat.y + collider.offsetY - collider.height / 2.0f;
        const bool pressed = scene.getResource<KeyboardInput>().isDown(Key::E);
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

    void place(Scene &scene, Entity rat, const Transform &ratTransform)
    {
        if (!scene.getResource<PrefabsManager>().hasPrefab(_prefab))
            return;
        const float radians = ratTransform.rotationY * TRAP_DEGREES_TO_RADIANS;
        const Entity trap = scene.getResource<PrefabsManager>().InstantiatePrefab(_prefab, scene);

        if (!scene.hasComponent<Transform>(trap))
            scene.addComponent(trap, Transform{});
        Transform &transform = scene.getComponent<Transform>(trap);
        // The rat faces (sin, -cos) of its rotation; the trap's long side follows it
        transform.x += ratTransform.x + std::sin(radians) * _distance;
        transform.y += _floorY;
        transform.z += ratTransform.z - std::cos(radians) * _distance;
        transform.rotationY += ratTransform.rotationY - 90.0f;
        if (scene.hasComponent<Collider3D>(trap))
            fit(scene.getComponent<Collider3D>(trap), transform);
        scene.getResource<TrapImmunity>().entries.push_back({rat, trap, _immunity});
        --_left;
        updateHud(scene);
    }

    void updateHud(Scene &scene)
    {
        for (const Panel &panel : _hud) {
            UIButton &button = scene.getComponent<UIButton>(panel.entity);

            if (panel.id == _counterId)
                button.content = "X" + std::to_string(_left);
            else if (panel.id == _keyId)
                show(button, _left > 0, panel);
        }
    }

    // A button can't be turned off: hidden, it has no size and no text
    static void show(UIButton &button, bool visible, const Panel &panel = {})
    {
        button.width = visible ? panel.width : 0;
        button.height = visible ? panel.height : 0;
        if (!visible)
            button.content.clear();
        else if (button.content.empty())
            button.content = panel.content;
    }

    // Collider around the turned trap: (x, z) turned like the renderer's yaw
    static void fit(Collider3D &collider, const Transform &trap)
    {
        const float radians = -trap.rotationY * TRAP_DEGREES_TO_RADIANS;
        const float sine = std::sin(radians);
        const float cosine = std::cos(radians);
        float minimumX = 0.0f;
        float maximumX = 0.0f;
        float minimumZ = 0.0f;
        float maximumZ = 0.0f;

        for (int corner = 0; corner < 4; ++corner) {
            const float x = (corner & 1 ? TRAP_MAX_X : TRAP_MIN_X) * trap.scaleX;
            const float z = (corner & 2 ? TRAP_MAX_Z : TRAP_MIN_Z) * trap.scaleZ;
            const float turnedX = cosine * x + sine * z;
            const float turnedZ = -sine * x + cosine * z;

            minimumX = corner == 0 ? turnedX : std::min(minimumX, turnedX);
            maximumX = corner == 0 ? turnedX : std::max(maximumX, turnedX);
            minimumZ = corner == 0 ? turnedZ : std::min(minimumZ, turnedZ);
            maximumZ = corner == 0 ? turnedZ : std::max(maximumZ, turnedZ);
        }
        collider.width = maximumX - minimumX;
        collider.height = (TRAP_MAX_Y - TRAP_MIN_Y) * trap.scaleY;
        collider.depth = maximumZ - minimumZ;
        collider.offsetX = (minimumX + maximumX) / 2.0f;
        collider.offsetY = (TRAP_MIN_Y + TRAP_MAX_Y) / 2.0f * trap.scaleY;
        collider.offsetZ = (minimumZ + maximumZ) / 2.0f;
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
    int _left = 3;
    float _distance = 12.0f;
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
