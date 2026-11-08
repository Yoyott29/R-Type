#ifndef BULLETCHARGESCRIPT_HPP_
#define BULLETCHARGESCRIPT_HPP_

#include "BuildVariantName.hpp"
#include "Collider2D.hpp"
#include "EnemyScript.hpp"
#include "GameScore.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include <string>
#include <vector>

inline const std::vector<std::string> BulletChargeScript_params = {"Damage"};

class BulletChargeScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
    void setParameter(const std::string &name, const std::string &value) override;
    int damage() const;

  private:
    static bool isClientSide(Scene &scene);

    int _damage = 1;
};

inline void BulletChargeScript::setParameter(const std::string &name, const std::string &value)
{
    if (name == "Damage")
        _damage = std::stoi(value);
}

inline int BulletChargeScript::damage() const { return _damage; }

inline bool BulletChargeScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline void BulletChargeScript::onStart(Scene &scene, Entity &entity)
{
    if (isClientSide(scene))
        return;
    scene.getComponent<Collider2D>(entity).onCollision = [&scene](Entity, Entity other) {
        if (!scene.isEntityAlive(other))
            return;
        scene.getResource<GameScore>().value += 100;
        EnemyScript::kill(scene, other);
    };
}

inline void BulletChargeScript::onUpdate(Scene &, Entity &) {}

#endif /* !BULLETCHARGESCRIPT_HPP_ */
