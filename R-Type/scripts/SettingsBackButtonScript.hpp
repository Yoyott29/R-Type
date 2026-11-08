#ifndef SETTINGSBACKBUTTONSCRIPT_HPP_
#define SETTINGSBACKBUTTONSCRIPT_HPP_

#include "IGringoButtonScript.hpp"
#include "Scene.hpp"
#include "Settings.hpp"

// Closes the settings panel, back to the lobby
class SettingsBackButtonScript : public IGringoButtonScript
{
  public:
    void onClick(Scene &scene, Entity entity) override;
};

inline void SettingsBackButtonScript::onClick(Scene &scene, Entity) { Settings::close(scene); }

#endif /* !SETTINGSBACKBUTTONSCRIPT_HPP_ */
