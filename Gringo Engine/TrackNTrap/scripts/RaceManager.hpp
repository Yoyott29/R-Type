#ifndef RACEMANAGER_HPP_
#define RACEMANAGER_HPP_

#include "GameState.hpp"
#include "ProjectComponentManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include "UIText.hpp"
#include <string>
#include <vector>

#define RACE_FINISH_X -150.0f
#define RACE_FINISH_MIN_Z 10.0f
#define RACE_FINISH_MAX_Z 60.0f

#define RACE_CHECKPOINTS 3
#define RACE_EAST_X 250.0f
#define RACE_NORTH_Z 380.0f
#define RACE_WEST_X -450.0f

inline const std::vector<std::string> RaceManager_params = {"rat", "laps", "timer", "lap", "win"};

class RaceManager : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "rat")
            _ratId = std::stoi(value);
        else if (name == "laps")
            _laps = std::stoi(value);
        else if (name == "timer")
            _timerId = std::stoi(value);
        else if (name == "lap")
            _lapId = std::stoi(value);
        else if (name == "win")
            _winId = std::stoi(value);
    }

    void onStart(Scene &scene, Entity &) override
    {
        if (!scene.hasResource<GameState>())
            scene.addResource<GameState>();

        show(scene, _timerId, "");
        show(scene, _lapId, "");
        show(scene, _winId, "");

        if (exists(scene, _ratId))
            _previousX = scene.getComponent<Transform>(entityOf(scene, _ratId)).x;
    }

    void onUpdate(Scene &scene, Entity &) override
    {
        GameState &state = scene.getResource<GameState>();

        if (!state.playing || state.finished)
            return;
        if (state.canMove)
            _time += FIXED_DELTA_TIME;
        show(scene, _timerId, clock(_time));
        show(scene, _lapId, "LAP " + std::to_string(_lap) + "/" + std::to_string(_laps));

        if (!exists(scene, _ratId))
            return;
        Transform &transform = scene.getComponent<Transform>(entityOf(scene, _ratId));

        if (state.respawns != _respawns) {
            _respawns = state.respawns;
            _checkpoint = 0;
            _previousX = transform.x;
            return;
        }

        if (_checkpoint < RACE_CHECKPOINTS && reached(_checkpoint, transform))
            ++_checkpoint;

        bool crossed = _previousX < RACE_FINISH_X && transform.x >= RACE_FINISH_X &&
                       transform.z >= RACE_FINISH_MIN_Z && transform.z <= RACE_FINISH_MAX_Z;
        _previousX = transform.x;

        if (!crossed || _checkpoint < RACE_CHECKPOINTS)
            return;

        _checkpoint = 0;

        if (_lap < _laps) {
            ++_lap;
            return;
        }

        state.finished = true;
        state.canMove = false;
        show(scene, _timerId, clock(_time));
        show(scene, _winId, "YOU WIN! " + clock(_time));
    }

  private:
    static bool reached(int checkpoint, Transform &rat)
    {
        if (checkpoint == 0)
            return rat.x > RACE_EAST_X;
        if (checkpoint == 1)
            return rat.z > RACE_NORTH_Z;
        return rat.x < RACE_WEST_X;
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

    static bool exists(Scene &scene, int id)
    {
        return scene.getResource<EditorEntityIds>().entities.count(id) > 0;
    }

    static Entity entityOf(Scene &scene, int id)
    {
        return scene.getResource<EditorEntityIds>().entities.at(id);
    }

    static void show(Scene &scene, int id, std::string content)
    {
        if (!exists(scene, id) || !scene.hasComponent<UIText>(entityOf(scene, id)))
            return;
        scene.getComponent<UIText>(entityOf(scene, id)).content = content;
    }

    int _ratId = -1;
    int _laps = 3;
    int _timerId = -1;
    int _lapId = -1;
    int _winId = -1;
    int _lap = 1;
    int _checkpoint = 0;
    int _respawns = 0;
    float _time = 0.0f;
    float _previousX = 0.0f;
};

#endif /* !RACEMANAGER_HPP_ */
