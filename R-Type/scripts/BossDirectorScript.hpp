#ifndef BOSSDIRECTORSCRIPT_HPP_
#define BOSSDIRECTORSCRIPT_HPP_

#include "BossState.hpp"
#include "BuildVariantName.hpp"
#include "EnemyShootScript.hpp"
#include "GameScore.hpp"
#include "Lobby.hpp"
#include "MatchState.hpp"
#include "Networked.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include "Velocity.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

class BossDirectorScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    enum class Phase { Waiting, Fighting, Dying };

    static constexpr std::int32_t SPAWN_SCORE = 10000;
    static constexpr std::int32_t KILL_REWARD = 5000;
    static constexpr std::int32_t NEXT_BOSS_STEP = 2 * SPAWN_SCORE;
    static constexpr int HP_PER_PLAYER = 100;
    static constexpr std::size_t STAGE_COUNT = 9;
    static constexpr float SPAWN_X = 2150.0f;
    static constexpr float STOP_X = 1600.0f;
    static constexpr float CENTER_Y = 540.0f;
    static constexpr float ENTER_SPEED = 150.0f;
    static constexpr float BOB_AMPLITUDE = 120.0f;
    static constexpr float BOB_SPEED = 0.6f;
    static constexpr float SHOOT_DELAY = 1.6f;
    static constexpr float SHOOT_DELAY_PER_STAGE = 0.1f;
    static constexpr float BULLET_SPEED = 450.0f;
    static constexpr float SPREAD = 0.25f;
    static constexpr float MOUTH_X = -130.0f;
    static constexpr float MOUTH_Y = -120.0f;
    static constexpr float DEATH_DELAY = 1.5f;

    static bool isClientSide(Scene &scene);
    static int countPlayers(Scene &scene);
    static Entity spawnPrefab(Scene &scene, const std::string &prefab, float x, float y);
    static void destroyIfAlive(Scene &scene, Entity entity);

    void reset(Scene &scene);
    void spawn(Scene &scene);
    void fight(Scene &scene);
    void move();
    std::size_t currentStage(Scene &scene) const;
    void updateStage(Scene &scene);
    void spawnBody(Scene &scene);
    void placeBody(Scene &scene) const;
    void shoot(Scene &scene);
    static void fireBullet(Scene &scene, const Transform &from, float angle);
    void die(Scene &scene);
    void finishFight(Scene &scene);
    void explodeAround(Scene &scene) const;
    void destroyParts(Scene &scene);

    Phase _phase = Phase::Waiting;
    std::int32_t _nextSpawnScore = SPAWN_SCORE;
    int _playerCount = 1;
    Transform _position{SPAWN_X, CENTER_Y};
    float _time = 0.0f;
    float _shootTimer = SHOOT_DELAY;
    float _deathTimer = 0.0f;
    std::size_t _stage = 0;
    std::optional<Entity> _body;
};

inline bool BossDirectorScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline int BossDirectorScript::countPlayers(Scene &scene)
{
    int count = 0;

    for (Entity entity : scene.view<Networked>())
        if (Lobby::isPlayer(scene, scene.getComponent<Networked>(entity)))
            count++;
    return std::max(1, count);
}

inline Entity BossDirectorScript::spawnPrefab(Scene &scene, const std::string &prefab, float x,
                                              float y)
{
    Entity entity = scene.getResource<PrefabsManager>().InstantiatePrefab(prefab, scene);
    Transform &position = scene.getComponent<Transform>(entity);

    position.x = x;
    position.y = y;
    return entity;
}

inline void BossDirectorScript::destroyIfAlive(Scene &scene, Entity entity)
{
    if (scene.isEntityAlive(entity))
        scene.queueDestroy(entity);
}

inline void BossDirectorScript::onStart(Scene &scene, Entity &)
{
    if (isClientSide(scene))
        return;
    if (!scene.hasResource<BossState>())
        scene.addResource<BossState>();
}

inline void BossDirectorScript::onUpdate(Scene &scene, Entity &)
{
    if (isClientSide(scene))
        return;
    if (isWaitingForMatch(scene)) {
        reset(scene);
        return;
    }
    if (_phase == Phase::Waiting) {
        if (scene.getResource<GameScore>().value >= _nextSpawnScore)
            spawn(scene);
        return;
    }
    if (_phase == Phase::Dying) {
        _deathTimer += FIXED_DELTA_TIME;
        if (_deathTimer >= DEATH_DELAY)
            finishFight(scene);
        return;
    }
    fight(scene);
}

inline void BossDirectorScript::reset(Scene &scene)
{
    _phase = Phase::Waiting;
    _nextSpawnScore = SPAWN_SCORE;
    _playerCount = countPlayers(scene);
    _body.reset();
    scene.getResource<BossState>() = BossState{};
}

inline void BossDirectorScript::spawn(Scene &scene)
{
    const int hp = HP_PER_PLAYER * _playerCount;

    scene.getResource<BossState>() = BossState{true, hp, hp};
    _position = Transform{SPAWN_X, CENTER_Y};
    _time = 0.0f;
    _shootTimer = SHOOT_DELAY;
    _stage = 0;
    spawnBody(scene);
    _phase = Phase::Fighting;
}

inline void BossDirectorScript::fight(Scene &scene)
{
    _time += FIXED_DELTA_TIME;
    if (scene.getResource<BossState>().hp <= 0) {
        die(scene);
        return;
    }
    move();
    updateStage(scene);
    placeBody(scene);
    shoot(scene);
}

inline void BossDirectorScript::move()
{
    _position.x = std::max(STOP_X, _position.x - ENTER_SPEED * FIXED_DELTA_TIME);
    _position.y = CENTER_Y + BOB_AMPLITUDE * std::sin(BOB_SPEED * _time);
}

inline std::size_t BossDirectorScript::currentStage(Scene &scene) const
{
    const BossState &state = scene.getResource<BossState>();
    const int damage = std::max(0, state.maxHp - state.hp);
    const std::size_t stage =
        static_cast<std::size_t>(damage) * STAGE_COUNT / static_cast<std::size_t>(state.maxHp);

    return std::min(STAGE_COUNT - 1, stage);
}

inline void BossDirectorScript::updateStage(Scene &scene)
{
    const std::size_t stage = currentStage(scene);

    if (stage == _stage)
        return;
    _stage = stage;
    if (_body)
        destroyIfAlive(scene, *_body);
    spawnBody(scene);
}

inline void BossDirectorScript::spawnBody(Scene &scene)
{
    _body = spawnPrefab(scene, "BossStage" + std::to_string(_stage + 1), _position.x, _position.y);
}

inline void BossDirectorScript::placeBody(Scene &scene) const
{
    if (!_body || !scene.isEntityAlive(*_body))
        return;
    Transform &position = scene.getComponent<Transform>(*_body);
    position.x = _position.x;
    position.y = _position.y;
}

inline void BossDirectorScript::shoot(Scene &scene)
{
    _shootTimer -= FIXED_DELTA_TIME;
    if (_shootTimer > 0.0f)
        return;
    _shootTimer = SHOOT_DELAY - SHOOT_DELAY_PER_STAGE * static_cast<float>(_stage);
    const Transform from{_position.x + MOUTH_X, _position.y + MOUTH_Y};
    const std::optional<Transform> target = EnemyShootScript::nearestPlayer(scene, from);
    if (!target)
        return;
    const float angle = std::atan2(target->y - from.y, target->x - from.x);
    fireBullet(scene, from, angle - SPREAD);
    fireBullet(scene, from, angle);
    fireBullet(scene, from, angle + SPREAD);
}

inline void BossDirectorScript::fireBullet(Scene &scene, const Transform &from, float angle)
{
    Entity bullet = spawnPrefab(scene, "BossBullet", from.x, from.y);

    scene.getComponent<Velocity>(bullet) =
        Velocity{std::cos(angle) * BULLET_SPEED, std::sin(angle) * BULLET_SPEED};
}

inline void BossDirectorScript::die(Scene &scene)
{
    scene.getResource<GameScore>().value += KILL_REWARD;
    _nextSpawnScore = scene.getResource<GameScore>().value + NEXT_BOSS_STEP;
    explodeAround(scene);
    destroyParts(scene);
    _deathTimer = 0.0f;
    _phase = Phase::Dying;
}

inline void BossDirectorScript::finishFight(Scene &scene)
{
    scene.getResource<BossState>() = BossState{};
    _phase = Phase::Waiting;
}

inline void BossDirectorScript::explodeAround(Scene &scene) const
{
    const std::array<Transform, 7> offsets{Transform{0.0f, 0.0f},    Transform{-90.0f, -130.0f},
                                           Transform{70.0f, -70.0f}, Transform{-40.0f, 50.0f},
                                           Transform{90.0f, 120.0f}, Transform{-110.0f, 160.0f},
                                           Transform{20.0f, -170.0f}};

    for (const Transform &offset : offsets)
        spawnPrefab(scene, "Explosion", _position.x + offset.x, _position.y + offset.y);
}

inline void BossDirectorScript::destroyParts(Scene &scene)
{
    if (_body)
        destroyIfAlive(scene, *_body);
    _body.reset();
}

#endif /* !BOSSDIRECTORSCRIPT_HPP_ */
