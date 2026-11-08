#ifndef DIFFICULTY_HPP_
#define DIFFICULTY_HPP_

#include "ActionInput.hpp"
#include "Lobby.hpp"
#include "Scene.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

enum class DifficultyLevel : std::uint8_t { Easy, Normal, Hard };

inline constexpr DifficultyLevel DEFAULT_DIFFICULTY = DifficultyLevel::Normal;

// Gameplay multipliers of one level, at the level's index in Difficulties::TABLE; 1.0 keeps the
// script's own value
struct DifficultySettings {
    std::string label;
    std::string markerPrefab;
    float fireCooldownScale;
    float bulletSpeedScale;
    float waveSizeScale;
    float waveDelayScale;
};

// Server: the level of the current or next match
struct Difficulty {
    DifficultyLevel level = DEFAULT_DIFFICULTY;
};

// Client: the level the local player asked for, sent on the difficulty action bits
struct DifficultyRequest {
    DifficultyLevel level = DEFAULT_DIFFICULTY;
};

namespace Difficulties
{
// Two action bits carry the requested level + 1, so 0 means "no request"
inline constexpr std::size_t ACTION_LOW_BIT = 6;
inline constexpr std::size_t ACTION_HIGH_BIT = 7;
inline constexpr std::size_t LOW_BIT_MASK = 1U;
inline constexpr std::size_t HIGH_BIT_MASK = 2U;
inline constexpr std::size_t NO_REQUEST = 0;
inline constexpr std::size_t LEVEL_COUNT = 3;
static_assert(LEVEL_COUNT <= (LOW_BIT_MASK | HIGH_BIT_MASK), "every level + 1 must fit in 2 bits");

inline const std::array<DifficultySettings, LEVEL_COUNT> TABLE = {{
    {"EASY", "DifficultyEasy", 1.5f, 0.75f, 0.7f, 1.5f},
    {"NORMAL", "DifficultyNormal", 1.0f, 1.0f, 1.0f, 1.0f},
    {"HARD", "DifficultyHard", 0.6f, 1.3f, 1.4f, 0.6f},
}};

inline const DifficultySettings &settingsOf(DifficultyLevel level)
{
    return TABLE[static_cast<std::size_t>(level)];
}

// Settings of the server's level, the default level when there is none (client side)
inline const DifficultySettings &current(Scene &scene)
{
    if (!scene.hasResource<Difficulty>())
        return settingsOf(DEFAULT_DIFFICULTY);
    return settingsOf(scene.getResource<Difficulty>().level);
}

inline DifficultyLevel next(DifficultyLevel level)
{
    return static_cast<DifficultyLevel>((static_cast<std::size_t>(level) + 1) % LEVEL_COUNT);
}

inline void encode(ActionInput &input, DifficultyLevel level)
{
    const std::size_t value = static_cast<std::size_t>(level) + 1;
    input.active.set(ACTION_LOW_BIT, (value & LOW_BIT_MASK) != 0);
    input.active.set(ACTION_HIGH_BIT, (value & HIGH_BIT_MASK) != 0);
}

inline std::optional<DifficultyLevel> decode(const ActionInput &input)
{
    const std::size_t value = (input.isActive(ACTION_LOW_BIT) ? LOW_BIT_MASK : 0) |
                              (input.isActive(ACTION_HIGH_BIT) ? HIGH_BIT_MASK : 0);
    if (value == NO_REQUEST)
        return std::nullopt;
    return static_cast<DifficultyLevel>(value - 1);
}

// The level shown by the replicated marker the server spawns in the lobby
inline std::optional<DifficultyLevel> fromMarker(Scene &scene)
{
    for (std::size_t index = 0; index < LEVEL_COUNT; ++index)
        if (Lobby::findByPrefab(scene, TABLE[index].markerPrefab))
            return static_cast<DifficultyLevel>(index);
    return std::nullopt;
}
} // namespace Difficulties

#endif /* !DIFFICULTY_HPP_ */
