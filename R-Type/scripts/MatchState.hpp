#ifndef MATCHSTATE_HPP_
#define MATCHSTATE_HPP_

#include "Scene.hpp"

struct MatchState {
    bool started = false;
};

inline bool isWaitingForMatch(Scene &scene)
{
    return scene.hasResource<MatchState>() && !scene.getResource<MatchState>().started;
}

#endif /* !MATCHSTATE_HPP_ */
