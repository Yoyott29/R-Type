#ifndef TRAPIMMUNITY_HPP_
#define TRAPIMMUNITY_HPP_

#include "Entity.hpp"
#include <algorithm>
#include <vector>

// Traps that can't kill the rat that placed them yet: one entry per trap, for "time" seconds
struct TrapImmunity {
    struct Entry {
        Entity rat;
        Entity trap;
        float time;
    };

    std::vector<Entry> entries;

    bool covers(Entity rat, Entity trap) const
    {
        return std::any_of(entries.begin(), entries.end(), [rat, trap](const Entry &entry) {
            return entry.rat == rat && entry.trap == trap;
        });
    }

    void elapse(float seconds)
    {
        for (Entry &entry : entries)
            entry.time -= seconds;
        std::erase_if(entries, [](const Entry &entry) { return entry.time <= 0.0f; });
    }
};

#endif /* !TRAPIMMUNITY_HPP_ */
