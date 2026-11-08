#ifndef SCOREBOARDSCRIPT_HPP_
#define SCOREBOARDSCRIPT_HPP_

#include "GameScore.hpp"
#include "Lobby.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "UIText.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

class ScoreboardScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static constexpr std::size_t MAX_SCORES = 5;

    void trackMatch(Scene &scene);
    void saveScore();
    std::string content() const;

    bool _wasInMatch = false;
    std::int32_t _lastScore = 0;
    std::vector<std::int32_t> _scores;
};

inline void ScoreboardScript::onStart(Scene &, Entity &) {}

inline void ScoreboardScript::onUpdate(Scene &scene, Entity &entity)
{
    if (!Lobby::isClient(scene))
        return;
    trackMatch(scene);
    scene.getComponent<UIText>(entity).content = content();
}

inline void ScoreboardScript::trackMatch(Scene &scene)
{
    const bool inMatch = Lobby::findByPrefab(scene, Lobby::MUSIC_PREFAB).has_value();

    if (inMatch)
        _lastScore = std::max(_lastScore, scene.getResource<GameScore>().value);
    if (_wasInMatch && !inMatch)
        saveScore();
    _wasInMatch = inMatch;
}

inline void ScoreboardScript::saveScore()
{
    _scores.push_back(_lastScore);
    std::sort(_scores.begin(), _scores.end(), std::greater<std::int32_t>());
    if (_scores.size() > MAX_SCORES)
        _scores.resize(MAX_SCORES);
    _lastScore = 0;
}

inline std::string ScoreboardScript::content() const
{
    std::string text = "ScoreBoard :";

    for (std::size_t i = 0; i < _scores.size(); i++)
        text += "\n" + std::to_string(i + 1) + ". " + std::to_string(_scores[i]);
    return text;
}

#endif /* !SCOREBOARDSCRIPT_HPP_ */
