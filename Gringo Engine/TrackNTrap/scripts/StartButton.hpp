#ifndef STARTBUTTON_HPP_
#define STARTBUTTON_HPP_

#include "IGringoButtonScript.hpp"
#include "Net.hpp"
#include "Scene.hpp"

// The lobby's Start button: the LobbyMenu then asks the server to start
class StartButton : public IGringoButtonScript
{
  public:
    void onClick(Scene &scene, Entity) override
    {
        if (!scene.hasResource<StartRequest>())
            scene.addResource<StartRequest>();
    }
};

#endif /* !STARTBUTTON_HPP_ */
