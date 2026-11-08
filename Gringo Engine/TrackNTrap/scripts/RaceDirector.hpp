#ifndef RACEDIRECTOR_HPP_
#define RACEDIRECTOR_HPP_

#include "AppAlive.hpp"
#include "GameState.hpp"
#include "Net.hpp"
#include "NetComponents.hpp"
#include "NetTransport.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include "Traps.hpp"
#include <map>
#include <string>
#include <vector>

#define RACE_FINISH_X -150.0f
#define RACE_FINISH_MIN_Z 10.0f
#define RACE_FINISH_MAX_Z 60.0f

#define RACE_CHECKPOINTS 3
#define RACE_EAST_X 250.0f
#define RACE_NORTH_Z 380.0f
#define RACE_WEST_X -450.0f

// "endDelay": seconds the result stays on screen before the server closes and kicks everybody
inline const std::vector<std::string> RaceDirector_params = {"laps", "countdown", "info",
                                                             "endDelay"};

class RaceDirector : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "laps")
            _laps = std::stoi(value);
        else if (name == "countdown")
            _countdown = std::stof(value);
        else if (name == "endDelay")
            _endDelay = std::stof(value);
        else if (name == "info")
            _infoPrefab = value;
    }

    void onStart(Scene &scene, Entity &) override
    {
        if (Net::isClient(scene))
            return;
        Net::replicateStatus(scene);
        if (!scene.hasResource<GameState>())
            scene.addResource<GameState>();
        if (!scene.hasResource<Traps>())
            scene.addResource<Traps>();
        _info = scene.getResource<PrefabsManager>().InstantiatePrefab(_infoPrefab, scene);
        RaceStatus status;
        status.laps = _laps;
        scene.addComponent(_info, status);
    }

    void onUpdate(Scene &scene, Entity &) override
    {
        if (Net::isClient(scene))
            return;
        RaceStatus &status = scene.getComponent<RaceStatus>(_info);
        GameState &state = scene.getResource<GameState>();

        if (hostLeft(scene)) {
            scene.getResource<AppAlive>().alive = false;
            return;
        }
        updateTraps(scene);
        if (status.phase == RACE_LOBBY && hostPressedStart(scene)) {
            status.phase = RACE_COUNTDOWN;
            status.countdown = _countdown;
            state.playing = true;
            scene.getResource<NetTransport>().setAcceptingPeers(false);
        } else if (status.phase == RACE_COUNTDOWN) {
            status.countdown -= FIXED_DELTA_TIME;
            if (status.countdown <= 0.0f) {
                status.countdown = 0.0f;
                status.phase = RACE_RUNNING;
                state.canMove = true;
            }
        } else if (status.phase == RACE_RUNNING) {
            status.time += FIXED_DELTA_TIME;
            countLaps(scene, status, state);
        } else if (status.phase == RACE_FINISHED) {
            // Time to read who won, then the server closes: its clients are disconnected on the
            // way out
            _endDelay -= FIXED_DELTA_TIME;
            if (_endDelay <= 0.0f)
                scene.getResource<AppAlive>().alive = false;
        }
    }

  private:
    struct Progress {
        int checkpoint = 0;
        float previousX = 0.0f;
        int respawns = 0;
    };

    static void updateTraps(Scene &scene)
    {
        Traps &traps = scene.getResource<Traps>();

        traps.elapse(FIXED_DELTA_TIME);
        for (Entity done : traps.takeDone())
            scene.queueDestroy(done);
    }

    bool hostLeft(Scene &scene)
    {
        if (_host == NO_PEER) {
            _host = Net::hostPeer(scene);
            return false;
        }
        for (Entity entity : scene.view<Networked>()) {
            if (Net::isRat(scene, entity) && scene.getComponent<Networked>(entity).owner == _host)
                return false;
        }
        return true;
    }

    static bool hostPressedStart(Scene &scene)
    {
        PeerId host = Net::hostPeer(scene);

        for (Entity entity : scene.view<Networked, RemoteInput>()) {
            if (scene.getComponent<Networked>(entity).owner != host)
                continue;
            if (scene.getComponent<RemoteInput>(entity).input.isActive(ACTION_START))
                return true;
        }
        return false;
    }

    void countLaps(Scene &scene, RaceStatus &status, GameState &state)
    {
        for (Entity rat : scene.view<RatStatus>()) {
            Transform &transform = scene.getComponent<Transform>(rat);
            RatStatus &ratStatus = scene.getComponent<RatStatus>(rat);

            if (_progress.count(rat) == 0)
                _progress[rat] = {0, transform.x, ratStatus.respawns};
            Progress &progress = _progress[rat];

            if (ratStatus.respawns != progress.respawns) {
                progress.respawns = ratStatus.respawns;
                progress.checkpoint = 0;
                progress.previousX = transform.x;
                continue;
            }
            if (progress.checkpoint < RACE_CHECKPOINTS && reached(progress.checkpoint, transform))
                progress.checkpoint += 1;
            bool crossed = progress.previousX < RACE_FINISH_X && transform.x >= RACE_FINISH_X &&
                           transform.z >= RACE_FINISH_MIN_Z && transform.z <= RACE_FINISH_MAX_Z;
            progress.previousX = transform.x;
            if (!crossed || progress.checkpoint < RACE_CHECKPOINTS)
                continue;
            progress.checkpoint = 0;
            if (ratStatus.lap < _laps) {
                ratStatus.lap += 1;
                continue;
            }

            status.phase = RACE_FINISHED;
            status.winner = Net::slot(scene, rat);
            state.canMove = false;
            state.finished = true;
            return;
        }
    }

    static bool reached(int checkpoint, Transform &rat)
    {
        if (checkpoint == 0)
            return rat.x > RACE_EAST_X;
        if (checkpoint == 1)
            return rat.z > RACE_NORTH_Z;
        return rat.x < RACE_WEST_X;
    }

    int _laps = 3;
    float _countdown = 3.0f;
    float _endDelay = 5.0f;
    std::string _infoPrefab = "RaceInfo";
    Entity _info{};
    PeerId _host = NO_PEER;
    std::map<Entity, Progress> _progress;
};

#endif /* !RACEDIRECTOR_HPP_ */
