#ifndef KEYBINDBUTTONSCRIPT_HPP_
#define KEYBINDBUTTONSCRIPT_HPP_

#include "IGringoButtonScript.hpp"
#include "PlayerScript.hpp"
#include "Scene.hpp"
#include "Settings.hpp"
#include <array>
#include <cstddef>
#include <string>
#include <vector>

inline const std::vector<std::string> KeybindButtonScript_params = {"Action"};

// Click: the next key pressed is bound to the button's action. SettingsMenuScript reads the key
// and writes the button's label
class KeybindButtonScript : public IGringoButtonScript
{
  public:
    void onClick(Scene &scene, Entity entity) override;
    void setParameter(const std::string &name, const std::string &value) override;

    std::size_t action() const { return CHOICES[_choice].action; }
    const std::string &label() const { return CHOICES[_choice].label; }

  private:
    struct Choice {
        std::string label;
        std::size_t action;
    };

    static inline const std::array<Choice, 5> CHOICES = {{
        {"UP", PlayerScript::ACTION_UP},
        {"DOWN", PlayerScript::ACTION_DOWN},
        {"LEFT", PlayerScript::ACTION_LEFT},
        {"RIGHT", PlayerScript::ACTION_RIGHT},
        {"SHOOT", PlayerScript::ACTION_CHARGE},
    }};

    std::size_t _choice = 0;
};

inline void KeybindButtonScript::setParameter(const std::string &name, const std::string &value)
{
    if (name != "Action")
        return;
    for (std::size_t index = 0; index < CHOICES.size(); ++index)
        if (CHOICES[index].label == value)
            _choice = index;
}

inline void KeybindButtonScript::onClick(Scene &scene, Entity)
{
    scene.addResource<RebindRequest>(RebindRequest{action()});
}

#endif /* !KEYBINDBUTTONSCRIPT_HPP_ */
