#ifndef HUD_HPP_
#define HUD_HPP_

#include "GameState.hpp"
#include "Net.hpp"
#include "ProjectComponentManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "UIButton.hpp"
#include "UIText.hpp"
#include <sstream>
#include <string>
#include <vector>

inline const std::vector<std::string> Hud_params = {"timer", "lap",       "win",
                                                    "traps", "trapCount", "trapKey"};

class Hud : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "timer")
            _timerId = std::stoi(value);
        else if (name == "lap")
            _lapId = std::stoi(value);
        else if (name == "win")
            _winId = std::stoi(value);
        else if (name == "traps")
            _trapIds = parseIds(value);
        else if (name == "trapCount")
            _trapCountId = std::stoi(value);
        else if (name == "trapKey")
            _trapKeyId = std::stoi(value);
    }

    void onStart(Scene &scene, Entity &) override
    {
        if (!Net::isClient(scene))
            return;
        Net::replicateStatus(scene);
        if (!scene.hasResource<GameState>())
            scene.addResource<GameState>();
        setText(scene, _timerId, "");
        setText(scene, _lapId, "");
        setText(scene, _winId, "");
        for (int id : _trapIds) {
            Entity entity;
            if (!find(scene, id, entity) || !scene.hasComponent<UIButton>(entity))
                continue;
            UIButton &button = scene.getComponent<UIButton>(entity);
            _traps.push_back({entity, id, button.width, button.height, button.content});
            hide(button);
        }
    }

    void onUpdate(Scene &scene, Entity &) override
    {
        if (!Net::isClient(scene))
            return;
        RaceStatus race;
        if (!Net::findRace(scene, race))
            return;
        GameState &state = scene.getResource<GameState>();
        state.playing = race.phase != RACE_LOBBY;
        state.canMove = race.phase == RACE_RUNNING;
        state.finished = race.phase == RACE_FINISHED;

        Entity rat;
        bool hasRat = Net::findMyRat(scene, rat) && scene.hasComponent<RatStatus>(rat);
        if (hasRat) {
            state.boosted = scene.getComponent<RatStatus>(rat).boosted;
            state.respawns = scene.getComponent<RatStatus>(rat).respawns;
        }
        if (!state.playing)
            return;
        setText(scene, _timerId, clock(race.time));
        if (hasRat) {
            RatStatus &status = scene.getComponent<RatStatus>(rat);
            setText(scene, _lapId,
                    "LAP " + std::to_string(status.lap) + "/" + std::to_string(race.laps));
            showTraps(scene, status.trapsLeft);
        }
        if (race.phase != RACE_FINISHED)
            return;
        if (hasRat && Net::slot(scene, rat) == race.winner)
            setText(scene, _winId, "YOU WIN! " + clock(race.time));
        else
            setText(scene, _winId,
                    "RAT " + std::to_string(race.winner) + " WINS! " + clock(race.time));
    }

  private:
    struct TrapButton {
        Entity entity;
        int id;
        int width;
        int height;
        std::string content;
    };

    void showTraps(Scene &scene, int left)
    {
        for (TrapButton &trap : _traps) {
            UIButton &button = scene.getComponent<UIButton>(trap.entity);

            if (trap.id == _trapKeyId && left <= 0) {
                hide(button);
                continue;
            }
            show(button, trap);
            if (trap.id == _trapCountId)
                button.content = "X" + std::to_string(left);
        }
    }

    static void hide(UIButton &button)
    {
        button.width = 0;
        button.height = 0;
        button.content.clear();
    }

    static void show(UIButton &button, TrapButton &trap)
    {
        button.width = trap.width;
        button.height = trap.height;
        if (button.content.empty())
            button.content = trap.content;
    }

    static std::string clock(float time)
    {
        int hundredths = static_cast<int>(time * 100.0f);
        int minutes = hundredths / 6000;
        int seconds = hundredths / 100 % 60;
        int rest = hundredths % 100;

        return std::to_string(minutes) + ":" + twoDigits(seconds) + "." + twoDigits(rest);
    }

    static std::string twoDigits(int number)
    {
        if (number < 10)
            return "0" + std::to_string(number);
        return std::to_string(number);
    }

    static bool find(Scene &scene, int id, Entity &entity)
    {
        EditorEntityIds &ids = scene.getResource<EditorEntityIds>();

        if (ids.entities.count(id) == 0)
            return false;
        entity = ids.entities.at(id);
        return true;
    }

    static void setText(Scene &scene, int id, std::string content)
    {
        Entity entity;
        if (find(scene, id, entity) && scene.hasComponent<UIText>(entity))
            scene.getComponent<UIText>(entity).content = content;
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

    int _timerId = -1;
    int _lapId = -1;
    int _winId = -1;
    std::vector<int> _trapIds;
    int _trapCountId = -1;
    int _trapKeyId = -1;
    std::vector<TrapButton> _traps;
};

#endif /* !HUD_HPP_ */
