#ifndef PLAYBUTTON_HPP_
#define PLAYBUTTON_HPP_

#include "Camera.hpp"
#include "GameState.hpp"
#include "IGringoButtonScript.hpp"
#include "IRenderer.hpp"
#include "ProjectComponentManager.hpp"
#include "Scene.hpp"
#include <functional>
#include <string>
#include <vector>

inline const std::vector<std::string> PlayButton_params = {"Menu", "MenuCamera", "RatCamera"};

class PlayButton : public IGringoButtonScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "Menu")
            _menu = std::stoi(value);
        else if (name == "MenuCamera")
            _menuCamera = std::stoi(value);
        else if (name == "RatCamera")
            _ratCamera = std::stoi(value);
    }

    void onClick(Scene &scene, Entity) override
    {
        if (!scene.hasResource<GameState>())
            return;
        const EditorEntityIds &ids = scene.getResource<EditorEntityIds>();

        scene.getResource<GameState>().playing = true;
        scene.getComponent<Camera>(ids.find(_menuCamera).value()).active = false;
        scene.getComponent<Camera>(ids.find(_ratCamera).value()).active = true;
        scene.getResource<std::reference_wrapper<IRenderer>>().get().setCursorLocked(true);
        scene.queueDestroy(ids.find(_menu).value());
    }

  private:
    int _menu = -1;
    int _menuCamera = -1;
    int _ratCamera = -1;
};

#endif /* !PLAYBUTTON_HPP_ */
