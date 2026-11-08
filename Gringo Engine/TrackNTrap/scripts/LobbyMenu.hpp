#ifndef LOBBYMENU_HPP_
#define LOBBYMENU_HPP_

#include "ActionInput.hpp"
#include "Camera.hpp"
#include "GameState.hpp"
#include "IRenderer.hpp"
#include "Net.hpp"
#include "ProjectComponentManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "UIButton.hpp"
#include "UIText.hpp"
#include <functional>
#include <sstream>
#include <string>
#include <vector>

inline const std::vector<std::string> LobbyMenu_params = {"menuCamera", "ratCamera", "startButton",
                                                          "subtitle", "slots"};

class LobbyMenu : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "menuCamera")
            _menuCameraId = std::stoi(value);
        else if (name == "ratCamera")
            _ratCameraId = std::stoi(value);
        else if (name == "startButton")
            _startButtonId = std::stoi(value);
        else if (name == "subtitle")
            _subtitleId = std::stoi(value);
        else if (name == "slots")
            _slotIds = parseIds(value);
    }

    void onStart(Scene &scene, Entity &) override
    {
        if (!Net::isClient(scene))
            return;
        if (!scene.hasResource<GameState>())
            scene.addResource<GameState>();
        Entity button;
        if (find(scene, _startButtonId, button) && scene.hasComponent<UIButton>(button)) {
            _buttonWidth = scene.getComponent<UIButton>(button).width;
            _buttonHeight = scene.getComponent<UIButton>(button).height;
            _buttonText = scene.getComponent<UIButton>(button).content;
        }
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        if (!Net::isClient(scene))
            return;
        if (scene.getResource<GameState>().playing) {
            startRace(scene);
            scene.queueDestroy(entity);
            return;
        }
        bool connected = Net::myPeer(scene) != NO_PEER;
        bool host = connected && Net::hostPeer(scene) == Net::myPeer(scene);

        if (!connected)
            setSubtitle(scene, "Connecting to the server...");
        else if (host)
            setSubtitle(scene, "You are the host: click START when everybody is in");
        else
            setSubtitle(scene, "Waiting for the host to start...");
        showStartButton(scene, host);
        showSlots(scene);
        if (host && scene.hasResource<StartRequest>() && scene.hasResource<ActionInput>())
            scene.getResource<ActionInput>().active.set(ACTION_START);
    }

  private:
    static bool find(Scene &scene, int id, Entity &entity)
    {
        EditorEntityIds &ids = scene.getResource<EditorEntityIds>();

        if (ids.entities.count(id) == 0)
            return false;
        entity = ids.entities.at(id);
        return true;
    }

    void showSlots(Scene &scene)
    {
        for (int i = 0; i < static_cast<int>(_slotIds.size()); ++i) {
            Entity text;
            if (find(scene, _slotIds.at(i), text) && scene.hasComponent<UIText>(text))
                scene.getComponent<UIText>(text).content =
                    "P" + std::to_string(i + 1) + "  " + slotStatus(scene, i + 1);
        }
    }

    static std::string slotStatus(Scene &scene, int slot)
    {
        for (Entity entity : scene.view<Networked>()) {
            if (!Net::isRat(scene, entity) || Net::slot(scene, entity) != slot)
                continue;
            PeerId owner = scene.getComponent<Networked>(entity).owner;
            std::string text = "READY";
            if (owner == Net::hostPeer(scene))
                text = "HOST";
            if (owner == Net::myPeer(scene))
                text += " (YOU)";
            return text;
        }
        return "EMPTY";
    }

    static std::vector<int> parseIds(const std::string &value)
    {
        std::vector<int> ids;
        std::stringstream stream(value);
        std::string id;

        while (std::getline(stream, id, ',')) {
            if (!id.empty())
                ids.push_back(std::stoi(id));
        }
        return ids;
    }

    void startRace(Scene &scene)
    {
        Entity camera;

        if (find(scene, _menuCameraId, camera) && scene.hasComponent<Camera>(camera))
            scene.getComponent<Camera>(camera).active = false;
        if (find(scene, _ratCameraId, camera) && scene.hasComponent<Camera>(camera))
            scene.getComponent<Camera>(camera).active = true;
        scene.getResource<std::reference_wrapper<IRenderer>>().get().setCursorLocked(true);
    }

    void setSubtitle(Scene &scene, std::string text)
    {
        Entity subtitle;
        if (find(scene, _subtitleId, subtitle) && scene.hasComponent<UIText>(subtitle))
            scene.getComponent<UIText>(subtitle).content = text;
    }

    void showStartButton(Scene &scene, bool visible)
    {
        Entity entity;
        if (!find(scene, _startButtonId, entity) || !scene.hasComponent<UIButton>(entity))
            return;
        UIButton &button = scene.getComponent<UIButton>(entity);
        if (visible) {
            button.width = _buttonWidth;
            button.height = _buttonHeight;
            button.content = _buttonText;
        } else {
            button.width = 0;
            button.height = 0;
            button.content.clear();
        }
    }

    int _menuCameraId = -1;
    int _ratCameraId = -1;
    int _startButtonId = -1;
    int _subtitleId = -1;
    std::vector<int> _slotIds;
    int _buttonWidth = 0;
    int _buttonHeight = 0;
    std::string _buttonText;
};

#endif /* !LOBBYMENU_HPP_ */
