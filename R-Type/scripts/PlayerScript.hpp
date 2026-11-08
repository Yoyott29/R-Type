#ifndef PLAYERSCRIPT_HPP_
#define PLAYERSCRIPT_HPP_

#include "ActionInput.hpp"
#include "BuildVariantName.hpp"
#include "Collider2D.hpp"
#include "MatchState.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Shape.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#ifdef GRINGO_USE_INPUTHANDLER
#include "InputBindings.hpp"
#endif
#ifdef GRINGO_USE_NETWORK
#include "NetComponents.hpp"
#endif

inline const std::vector<std::string> PlayerScript_params = {"speed", "health"};

class PlayerScript : public GringoEntityScript
{
  public:
    static constexpr std::size_t ACTION_UP = 0;
    static constexpr std::size_t ACTION_DOWN = 1;
    static constexpr std::size_t ACTION_LEFT = 2;
    static constexpr std::size_t ACTION_RIGHT = 3;
    static constexpr std::size_t ACTION_CHARGE = 5;
    static constexpr std::uint32_t PLAYER_LAYER = 1;
    static constexpr std::uint32_t DEAD_LAYER = 0;

    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
    void setParameter(const std::string &name, const std::string &value) override;

    static void kill(Scene &scene, Entity player);
    static PlayerScript *from(Scene &scene, Entity player);
    void enableDiagonalShot();

  private:
    static constexpr float SHOOT_COOLDOWN = 0.13f;
    static constexpr float BULLET_OFFSET_X = 25.0f;
    static constexpr float SCREEN_WIDTH = 1920.0f;
    static constexpr float SCREEN_HEIGHT = 1080.0f;
    static constexpr float DEAD_POSITION = -1000.0f;
    static constexpr float CHARGE_LEVEL_TIME = 0.3f;
    static constexpr std::size_t MAX_CHARGE_LEVEL = 5;
    static constexpr float CHARGE_HALF_WIDTH = 24.0f;

    static bool isDead(Scene &scene, Entity entity);
    void revive(Scene &scene, Entity entity);
    static bool isClientSide(Scene &scene);
    static void bindKeys(Scene &scene);
    static ActionInput readInput(Scene &scene, Entity entity);
    static Transform halfSize(const Collider2D &collider);
    static void clampToScreen(Scene &scene, Entity entity);
    void move(Scene &scene, Entity entity, const ActionInput &input) const;
    void shoot(Scene &scene, Entity entity);
    void releaseCharge(Scene &scene, Entity entity);
    static void spawnBullet(Scene &scene, Entity entity, const std::string &prefab, float offsetX);

    float _speed = 400.0f;
    float _shootCooldown = 0.0f;
    float _chargeTime = 0.0f;
    bool _diagonalShot = false;
    Transform _spawn{0.0f, 0.0f};
};

inline void PlayerScript::setParameter(const std::string &name, const std::string &value)
{
    if (name == "speed")
        _speed = std::stof(value);
}

inline bool PlayerScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline void PlayerScript::bindKeys([[maybe_unused]] Scene &scene)
{
#ifdef GRINGO_USE_INPUTHANDLER
    if (scene.hasResource<InputBindings>())
        return;
    InputBindings &bindings = scene.addResource<InputBindings>();
    bindings.bind(Key::Up, ACTION_UP);
    bindings.bind(Key::Z, ACTION_UP);
    bindings.bind(Key::Down, ACTION_DOWN);
    bindings.bind(Key::S, ACTION_DOWN);
    bindings.bind(Key::Left, ACTION_LEFT);
    bindings.bind(Key::Q, ACTION_LEFT);
    bindings.bind(Key::Right, ACTION_RIGHT);
    bindings.bind(Key::D, ACTION_RIGHT);
    bindings.bind(Key::Space, ACTION_CHARGE);
#endif
}

inline void PlayerScript::onStart(Scene &scene, Entity &entity)
{
    bindKeys(scene);
    _spawn = scene.getComponent<Transform>(entity);
}

inline void PlayerScript::kill(Scene &scene, Entity player)
{
    scene.getComponent<Collider2D>(player).layer = DEAD_LAYER;
    Transform &position = scene.getComponent<Transform>(player);
    position.x = DEAD_POSITION;
    position.y = DEAD_POSITION;
}

inline bool PlayerScript::isDead(Scene &scene, Entity entity)
{
    return scene.getComponent<Collider2D>(entity).layer == DEAD_LAYER;
}

inline void PlayerScript::revive(Scene &scene, Entity entity)
{
    if (!isDead(scene, entity))
        return;
    _chargeTime = 0.0f;
    _diagonalShot = false;
    scene.getComponent<Collider2D>(entity).layer = PLAYER_LAYER;
    Transform &position = scene.getComponent<Transform>(entity);
    position.x = _spawn.x;
    position.y = _spawn.y;
}

inline void PlayerScript::onUpdate(Scene &scene, Entity &entity)
{
    if (isClientSide(scene))
        return;
    if (isWaitingForMatch(scene)) {
        revive(scene, entity);
        return;
    }
    if (isDead(scene, entity))
        return;
    _shootCooldown = std::max(0.0f, _shootCooldown - FIXED_DELTA_TIME);
    const ActionInput input = readInput(scene, entity);
    move(scene, entity, input);
    clampToScreen(scene, entity);
    if (input.isActive(ACTION_CHARGE)) {
        _chargeTime += FIXED_DELTA_TIME;
        return;
    }
    releaseCharge(scene, entity);
    shoot(scene, entity);
}

inline ActionInput PlayerScript::readInput(Scene &scene, [[maybe_unused]] Entity entity)
{
#ifdef GRINGO_USE_NETWORK
    if (scene.hasComponent<RemoteInput>(entity))
        return scene.getComponent<RemoteInput>(entity).input;
#endif
    if (scene.hasResource<ActionInput>())
        return scene.getResource<ActionInput>();
    return ActionInput{};
}

inline void PlayerScript::move(Scene &scene, Entity entity, const ActionInput &input) const
{
    Transform &position = scene.getComponent<Transform>(entity);
    const float step = _speed * FIXED_DELTA_TIME;

    if (input.isActive(ACTION_UP))
        position.y -= step;
    if (input.isActive(ACTION_DOWN))
        position.y += step;
    if (input.isActive(ACTION_LEFT))
        position.x -= step;
    if (input.isActive(ACTION_RIGHT))
        position.x += step;
}

inline Transform PlayerScript::halfSize(const Collider2D &collider)
{
    if (std::holds_alternative<BoxShape>(collider.shape)) {
        const BoxShape &box = std::get<BoxShape>(collider.shape);
        return Transform{box.width / 2.0f, box.height / 2.0f};
    }
    const float radius = std::get<CircleShape>(collider.shape).radius;
    return Transform{radius, radius};
}

inline void PlayerScript::clampToScreen(Scene &scene, Entity entity)
{
    Transform &position = scene.getComponent<Transform>(entity);
    const Transform half = halfSize(scene.getComponent<Collider2D>(entity));

    position.x = std::clamp(position.x, half.x, SCREEN_WIDTH - half.x);
    position.y = std::clamp(position.y, half.y, SCREEN_HEIGHT - half.y);
}

inline void PlayerScript::shoot(Scene &scene, Entity entity)
{
    if (_shootCooldown > 0.0f)
        return;
    spawnBullet(scene, entity, "PlayerBullet", BULLET_OFFSET_X);
    if (_diagonalShot) {
        spawnBullet(scene, entity, "PlayerBulletUp", BULLET_OFFSET_X);
        spawnBullet(scene, entity, "PlayerBulletDown", BULLET_OFFSET_X);
    }
    _shootCooldown = SHOOT_COOLDOWN;
}

inline PlayerScript *PlayerScript::from(Scene &scene, Entity player)
{
    for (std::size_t i = 0; i < scene.getComponentCount<Script>(player); i++) {
        const std::shared_ptr<PlayerScript> script = std::dynamic_pointer_cast<PlayerScript>(
            scene.getComponent<Script>(player, i).gringoEntityScript);
        if (script)
            return script.get();
    }
    return nullptr;
}

inline void PlayerScript::enableDiagonalShot() { _diagonalShot = true; }

inline void PlayerScript::releaseCharge(Scene &scene, Entity entity)
{
    const std::size_t level =
        std::min(MAX_CHARGE_LEVEL, static_cast<std::size_t>(_chargeTime / CHARGE_LEVEL_TIME));
    _chargeTime = 0.0f;
    if (level == 0)
        return;
    const float offset = BULLET_OFFSET_X + CHARGE_HALF_WIDTH * static_cast<float>(level);
    spawnBullet(scene, entity, "BulletCharge" + std::to_string(level), offset);
    _shootCooldown = SHOOT_COOLDOWN;
}

inline void PlayerScript::spawnBullet(Scene &scene, Entity entity, const std::string &prefab,
                                      float offsetX)
{
    const Transform origin = scene.getComponent<Transform>(entity);
    Entity bullet = scene.getResource<PrefabsManager>().InstantiatePrefab(prefab, scene);
    Transform &position = scene.getComponent<Transform>(bullet);

    position.x = origin.x + offsetX;
    position.y = origin.y;
}

#endif /* !PLAYERSCRIPT_HPP_ */
