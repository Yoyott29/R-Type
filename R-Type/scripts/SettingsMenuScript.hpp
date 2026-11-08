#ifndef SETTINGSMENUSCRIPT_HPP_
#define SETTINGSMENUSCRIPT_HPP_

#include "KeybindButtonScript.hpp"
#include "Lobby.hpp"
#include "PlayerScript.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Settings.hpp"
#include "UIButton.hpp"
#include <bitset>
#include <cstddef>
#include <optional>
#include <string>
#ifdef GRINGO_USE_INPUTHANDLER
#include "InputBindings.hpp"
#include "KeyboardInput.hpp"
#endif

// The settings panel (keybinds, color filter): shown while SettingsOpen exists in the lobby.
// While a RebindRequest exists, the next key pressed is bound to its action, Escape cancels
class SettingsMenuScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static constexpr const char *WAITING_LABEL = "PRESS A KEY";
    static constexpr const char *UNBOUND_LABEL = "NONE";
    static constexpr const char *KEY_SEPARATOR = " / ";

#ifdef GRINGO_USE_INPUTHANDLER
    void readRebind(Scene &scene);
    static std::string keysOf(const InputBindings &bindings, std::size_t action);
    static void refreshLabels(Scene &scene);
#endif

    bool _hidden = false;
    std::bitset<Settings::KEY_COUNT> _previous;
};

inline void SettingsMenuScript::onStart(Scene &scene, Entity &entity)
{
    if (Lobby::isClient(scene))
        Lobby::setHidden(scene, entity, _hidden, true);
}

inline void SettingsMenuScript::onUpdate(Scene &scene, Entity &entity)
{
    if (!Lobby::isClient(scene))
        return;
    if (Lobby::findByPrefab(scene, Lobby::MUSIC_PREFAB).has_value())
        Settings::close(scene);
    const bool open = scene.hasResource<SettingsOpen>();
    Lobby::setHidden(scene, entity, _hidden, !open);
#ifdef GRINGO_USE_INPUTHANDLER
    PlayerScript::bindKeys(scene);
    readRebind(scene);
    if (open)
        refreshLabels(scene);
#endif
}

#ifdef GRINGO_USE_INPUTHANDLER
// Only keys that went down this frame count: one held since before the click is ignored
inline void SettingsMenuScript::readRebind(Scene &scene)
{
    if (!scene.hasResource<KeyboardInput>())
        return;
    const auto &pressed = scene.getResource<KeyboardInput>().pressed;
    const auto fresh = pressed & ~_previous;
    _previous = pressed;
    if (!scene.hasResource<RebindRequest>() || fresh.none())
        return;
    const std::size_t action = scene.getResource<RebindRequest>().action;
    scene.removeResource<RebindRequest>();
    std::size_t index = 0;
    while (!fresh.test(index))
        ++index;
    const Key key = static_cast<Key>(index);
    if (key == Key::Escape)
        return;
    InputBindings &bindings = scene.getResource<InputBindings>();
    std::erase_if(bindings.actions, [action](const auto &entry) { return entry.second == action; });
    bindings.bind(key, action);
}

inline std::string SettingsMenuScript::keysOf(const InputBindings &bindings, std::size_t action)
{
    std::string keys;
    for (const auto &[key, bound] : bindings.actions) {
        if (bound != action)
            continue;
        if (!keys.empty())
            keys += KEY_SEPARATOR;
        keys += Settings::keyName(key);
    }
    return keys.empty() ? std::string(UNBOUND_LABEL) : keys;
}

inline void SettingsMenuScript::refreshLabels(Scene &scene)
{
    const InputBindings &bindings = scene.getResource<InputBindings>();
    std::optional<std::size_t> waiting;
    if (scene.hasResource<RebindRequest>())
        waiting = scene.getResource<RebindRequest>().action;
    for (Entity entity : scene.view<UIButton>()) {
        UIButton &button = scene.getComponent<UIButton>(entity);
        const auto *keybind = dynamic_cast<const KeybindButtonScript *>(button.script.get());
        if (!keybind)
            continue;
        const std::size_t action = keybind->action();
        button.content = keybind->label() + ": " +
                         (waiting == action ? std::string(WAITING_LABEL) : keysOf(bindings, action));
    }
}
#endif

#endif /* !SETTINGSMENUSCRIPT_HPP_ */
