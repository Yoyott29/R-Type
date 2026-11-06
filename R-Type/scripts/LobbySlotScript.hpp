#ifndef LOBBYSLOTSCRIPT_HPP_
#define LOBBYSLOTSCRIPT_HPP_

#include "Lobby.hpp"
#include "NetworkConfig.hpp"
#include "Networked.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "UIText.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

inline const std::vector<std::string> LobbySlotScript_params = {"Slot"};

class LobbySlotScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
    void setParameter(const std::string &name, const std::string &value) override;

  private:
    std::string status(Scene &scene) const;

    std::size_t _slot = 0;
};

inline void LobbySlotScript::setParameter(const std::string &name, const std::string &value)
{
    if (name == "Slot")
        _slot = std::stoul(value);
}

inline void LobbySlotScript::onStart(Scene &, Entity &) {}

inline void LobbySlotScript::onUpdate(Scene &scene, Entity &entity)
{
    if (!Lobby::isClient(scene))
        return;
    scene.getComponent<UIText>(entity).content =
        "P" + std::to_string(_slot + 1) + "  " + status(scene);
}

inline std::string LobbySlotScript::status(Scene &scene) const
{
    const std::vector<std::string> &prefabs = scene.getResource<NetworkConfig>().playerPrefabs;
    if (_slot >= prefabs.size())
        return "EMPTY";
    const std::optional<Entity> player = Lobby::findByPrefab(scene, prefabs[_slot]);
    if (!player)
        return "EMPTY";
    if (scene.getComponent<Networked>(*player).owner == Lobby::hostPeer(scene))
        return "HOST";
    return "READY";
}

#endif /* !LOBBYSLOTSCRIPT_HPP_ */
