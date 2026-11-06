#ifndef PLAYBUTTON_HPP_
#define PLAYBUTTON_HPP_

#include "GameState.hpp"
#include "IGringoButtonScript.hpp"
#include "ProjectComponentManager.hpp"
#include "Scene.hpp"
#include <string>
#include <vector>

inline const std::vector<std::string> PlayButton_params = {"Menu"};

class PlayButton : public IGringoButtonScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "Menu")
            _menu = std::stoi(value);
    }

    void onClick(Scene &scene, Entity) override
    {
        if (!scene.hasResource<GameState>())
            return;
        scene.getResource<GameState>().playing = true;

        auto menu = scene.getResource<EditorEntityIds>().find(_menu);

        if (menu != -1)
            scene.queueDestroy(*menu);
    }

  private:
    int _menu = -1;
};

#endif /* !PLAYBUTTON_HPP_ */
