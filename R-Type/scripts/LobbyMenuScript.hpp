#ifndef LOBBYMENUSCRIPT_HPP_
#define LOBBYMENUSCRIPT_HPP_

#include "ActionInput.hpp"
#include "Difficulty.hpp"
#include "DifficultyButtonScript.hpp"
#include "Lobby.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Settings.hpp"
#include "UIButton.hpp"
#include <optional>
#include <string>

class LobbyMenuScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static inline const std::string DIFFICULTY_LABEL_PREFIX = "DIFFICULTY: ";

    void refreshDifficultyLabel(Scene &scene);

    bool _hidden = false;
    std::optional<DifficultyLevel> _shownDifficulty;
};

inline void LobbyMenuScript::onStart(Scene &, Entity &) {}

inline void LobbyMenuScript::onUpdate(Scene &scene, Entity &entity)
{
    if (!Lobby::isClient(scene))
        return;
    const bool inMatch = Lobby::findByPrefab(scene, Lobby::MUSIC_PREFAB).has_value();
    const bool hidden = inMatch || scene.hasResource<SettingsOpen>();
    if (Lobby::setHidden(scene, entity, _hidden, hidden) && hidden)
        scene.removeResource<StartRequest>();
    if (hidden)
        return;
    refreshDifficultyLabel(scene);
    if (!scene.hasResource<ActionInput>())
        return;
    ActionInput &input = scene.getResource<ActionInput>();
    if (scene.hasResource<StartRequest>())
        input.active.set(Lobby::ACTION_START);
    if (scene.hasResource<DifficultyRequest>())
        Difficulties::encode(input, scene.getResource<DifficultyRequest>().level);
}

inline void LobbyMenuScript::refreshDifficultyLabel(Scene &scene)
{
    const DifficultyLevel level = Difficulties::fromMarker(scene).value_or(DEFAULT_DIFFICULTY);
    if (_shownDifficulty == level)
        return;
    _shownDifficulty = level;
    for (Entity entity : scene.view<UIButton>()) {
        UIButton &button = scene.getComponent<UIButton>(entity);
        if (dynamic_cast<const DifficultyButtonScript *>(button.script.get()))
            button.content = DIFFICULTY_LABEL_PREFIX + Difficulties::settingsOf(level).label;
    }
}

#endif /* !LOBBYMENUSCRIPT_HPP_ */
