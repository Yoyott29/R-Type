#ifndef COUNTDOWNSCRIPT_HPP_
#define COUNTDOWNSCRIPT_HPP_

#include "GameState.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "UIText.hpp"
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#define FADE_START 0.7f
#define GO_SECONDS 1.0f

inline std::vector<std::string> CountdownScript_params = {"seconds", "startSize"};

class CountdownScript : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "seconds")
            _remaining = std::stof(value);
        else if (name == "startSize")
            _startSize = std::stof(value);
    }

    void onStart(Scene &scene, Entity &entity) override
    {
        if (!scene.hasResource<GameState>())
            scene.addResource<GameState>();
        _endSize = static_cast<float>(scene.getComponent<UIText>(entity).fontSize);
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        GameState &state = scene.getResource<GameState>();

        if (!state.playing || _finished)
            return;
        UIText &text = scene.getComponent<UIText>(entity);

        _remaining -= FIXED_DELTA_TIME;
        if (_remaining > 0.0f) {
            float number = std::ceil(_remaining);

            text.content = std::to_string(static_cast<int>(number));
            zoomOut(text, number - _remaining);
            return;
        }
        state.canMove = true;
        if (_remaining > -GO_SECONDS) {
            text.content = "GO";
            zoomOut(text, -_remaining / GO_SECONDS);
            return;
        }
        text.content.clear();
        _finished = true;
    }

  private:
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
};

#endif /* !COUNTDOWNSCRIPT_HPP_ */
