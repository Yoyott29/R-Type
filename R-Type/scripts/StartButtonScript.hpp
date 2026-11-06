#ifndef STARTBUTTONSCRIPT_HPP_
#define STARTBUTTONSCRIPT_HPP_

#include "IGringoButtonScript.hpp"
#include "Lobby.hpp"
#include "Scene.hpp"

class StartButtonScript : public IGringoButtonScript
{
  public:
    void onClick(Scene &scene, Entity entity) override;
};

inline void StartButtonScript::onClick(Scene &scene, Entity)
{
    if (!scene.hasResource<StartRequest>())
        scene.addResource<StartRequest>();
}

#endif /* !STARTBUTTONSCRIPT_HPP_ */
