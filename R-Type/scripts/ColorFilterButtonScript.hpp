#ifndef COLORFILTERBUTTONSCRIPT_HPP_
#define COLORFILTERBUTTONSCRIPT_HPP_

#include "IGringoButtonScript.hpp"
#include "IRenderer.hpp"
#include "Scene.hpp"
#include "UIButton.hpp"
#include <array>
#include <cstddef>
#include <functional>
#include <string>

class ColorFilterButtonScript : public IGringoButtonScript
{
  public:
    void onClick(Scene &scene, Entity entity) override;

  private:
    struct Choice {
        ColorFilter filter;
        std::string label;
    };

    static inline const std::array<Choice, 4> CHOICES = {{
        {ColorFilter::None, "COLORS: NORMAL"},
        {ColorFilter::Protanopia, "COLORS: PROTANOPIA"},
        {ColorFilter::Deuteranopia, "COLORS: DEUTERANOPIA"},
        {ColorFilter::Tritanopia, "COLORS: TRITANOPIA"},
    }};

    std::size_t _current = 0;
};

inline void ColorFilterButtonScript::onClick(Scene &scene, Entity entity)
{
    if (!scene.hasResource<std::reference_wrapper<IRenderer>>())
        return;
    _current = (_current + 1) % CHOICES.size();
    scene.getResource<std::reference_wrapper<IRenderer>>().get().setColorFilter(
        CHOICES[_current].filter);
    scene.getComponent<UIButton>(entity).content = CHOICES[_current].label;
}

#endif /* !COLORFILTERBUTTONSCRIPT_HPP_ */
