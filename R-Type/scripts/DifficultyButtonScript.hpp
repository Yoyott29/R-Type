#ifndef DIFFICULTYBUTTONSCRIPT_HPP_
#define DIFFICULTYBUTTONSCRIPT_HPP_

#include "Difficulty.hpp"
#include "IGringoButtonScript.hpp"
#include "Scene.hpp"

// Click: asks for the next level. Only the host's request is applied by the server;
// LobbyMenuScript writes the button's label from the level the server replicates
class DifficultyButtonScript : public IGringoButtonScript
{
  public:
    void onClick(Scene &scene, Entity entity) override;
};

inline void DifficultyButtonScript::onClick(Scene &scene, Entity)
{
    if (!scene.hasResource<DifficultyRequest>())
        scene.addResource<DifficultyRequest>(
            DifficultyRequest{Difficulties::fromMarker(scene).value_or(DEFAULT_DIFFICULTY)});
    DifficultyLevel &level = scene.getResource<DifficultyRequest>().level;
    level = Difficulties::next(level);
}

#endif /* !DIFFICULTYBUTTONSCRIPT_HPP_ */
