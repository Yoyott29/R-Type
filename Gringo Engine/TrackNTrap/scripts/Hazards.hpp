#ifndef HAZARDS_HPP_
#define HAZARDS_HPP_

#include "Entity.hpp"
#include <map>

// What an obstacle does to a rat touching it. Not in the list: the rat goes back to the start
struct Hazard {
    enum class Effect { Slow, Stun };

    Effect effect;
    // Seconds it lasts
    float duration;
    // Slow only: speed times this
    float factor = 1.0f;
};

// The obstacles' hit boxes and what they do, filled by the obstacles' scripts on start
class Hazards
{
  public:
    void add(Entity entity, Hazard hazard) { _obstacles[entity] = hazard; }

    bool has(Entity entity) { return _obstacles.count(entity) > 0; }

    Hazard get(Entity entity) { return _obstacles.at(entity); }

  private:
    std::map<Entity, Hazard> _obstacles;
};

#endif /* !HAZARDS_HPP_ */
