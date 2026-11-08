#ifndef COUNTDOWNSCRIPT_HPP_
#define COUNTDOWNSCRIPT_HPP_

#include "AudioSource.hpp"
#include "GameState.hpp"
#include "Net.hpp"
#include "ProjectComponentManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "UIText.hpp"
#include <algorithm>
#include <cmath>
#include <optional>
#include <string>
#include <vector>

#define FADE_START 0.7f
#define GO_SECONDS 1.0f

inline std::vector<std::string> CountdownScript_params = {"seconds", "startSize", "music"};

class CountdownScript : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "seconds")
            _remaining = std::stof(value);
        else if (name == "startSize")
            _startSize = std::stof(value);
        else if (name == "music")
            _musicId = std::stoi(value);
    }

    void onStart(Scene &scene, Entity &entity) override
    {
        if (!Net::isClient(scene))
            return;
        if (!scene.hasResource<GameState>())
            scene.addResource<GameState>();
        _endSize = static_cast<float>(scene.getComponent<UIText>(entity).fontSize);
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        if (!Net::isClient(scene))
            return;
        GameState &state = scene.getResource<GameState>();
        RaceStatus race;

        if (!state.playing || _finished || !Net::findRace(scene, race))
            return;
        UIText &text = scene.getComponent<UIText>(entity);

        if (!_started) {
            _started = true;
            if (const std::optional<Entity> music = findMusic(scene))
                scene.getComponent<AudioSource>(music.value()).stop();
        }
        // The server counts: seconds left before GO, then below 0 the seconds since GO
        _remaining = race.countdown;
        if (race.phase != RACE_COUNTDOWN)
            _remaining = -race.time;
        if (_remaining > 0.0f) {
            float number = std::ceil(_remaining);

            if (number != _lastNumber) {
                _lastNumber = number;
                if (scene.hasComponent<AudioSource>(entity))
                    scene.getComponent<AudioSource>(entity).play();
            }
            text.content = std::to_string(static_cast<int>(number));
            zoomOut(text, number - _remaining);
            return;
        }
        if (!_musicStarted) {
            _musicStarted = true;
            if (const std::optional<Entity> music = findMusic(scene))
                scene.getComponent<AudioSource>(music.value()).play();
        }
        if (_remaining > -GO_SECONDS) {
            text.content = "GO";
            zoomOut(text, -_remaining / GO_SECONDS);
            return;
        }
        text.content.clear();
        _finished = true;
    }

  private:
    std::optional<Entity> findMusic(Scene &scene) const
    {
        const std::optional<Entity> music = scene.getResource<EditorEntityIds>().find(_musicId);
        if (!music || !scene.hasComponent<AudioSource>(*music))
            return std::nullopt;
        return music;
    }

    void zoomOut(UIText &text, float progress) const
    {
        float rest = 1.0f - progress;
        float zoom = rest * rest * rest;
        float fade = (1.0f - progress) / (1.0f - FADE_START);

        text.fontSize = static_cast<int>(_endSize + (_startSize - _endSize) * zoom);
        text.color.a = static_cast<unsigned char>(255.0f * std::min(fade, 1.0f));
    }

    float _remaining = 3.0f;
    float _startSize = 700.0f;
    float _endSize = 160.0f;
    bool _finished = false;
    bool _started = false;
    bool _musicStarted = false;
    float _lastNumber = 0.0f;
    int _musicId = -1;
};

#endif /* !COUNTDOWNSCRIPT_HPP_ */
