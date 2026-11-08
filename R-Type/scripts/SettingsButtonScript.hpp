#ifndef SETTINGSBUTTONSCRIPT_HPP_
#define SETTINGSBUTTONSCRIPT_HPP_

#include "IGringoButtonScript.hpp"
#include "Scene.hpp"
#include "Settings.hpp"

// Opens the settings panel
class SettingsButtonScript : public IGringoButtonScript
{
  public:
    void onClick(Scene &scene, Entity entity) override;
};

inline void SettingsButtonScript::onClick(Scene &scene, Entity)
{
    scene.addResource<SettingsOpen>();
}

#endif /* !SETTINGSBUTTONSCRIPT_HPP_ */
