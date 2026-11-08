#ifndef BOSSSTATE_HPP_
#define BOSSSTATE_HPP_

#include "Scene.hpp"
#include <cstdint>

struct BossState {
    bool active = false;
    int hp = 0;
    int maxHp = 0;
};

inline constexpr std::uint32_t BOSS_LAYER = 32;

inline bool isBossActive(Scene &scene)
{
    return scene.hasResource<BossState>() && scene.getResource<BossState>().active;
}

inline void hitBoss(Scene &scene, int damage)
{
    if (!isBossActive(scene))
        return;
    scene.getResource<BossState>().hp -= damage;
}

#endif /* !BOSSSTATE_HPP_ */
