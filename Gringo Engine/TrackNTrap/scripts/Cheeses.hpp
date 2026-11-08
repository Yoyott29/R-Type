#ifndef CHEESES_HPP_
#define CHEESES_HPP_

#include "Entity.hpp"
#include <vector>

// The cheeses on the track: a rat touching one eats it instead of dying. The CheeseSpawner takes
// the eaten ones away and puts new ones later
class Cheeses
{
  public:
    // A new cheese on the track
    void add(Entity cheese) { _active.push_back(cheese); }

    // True if it was a cheese still on the track: it's now eaten, and only once
    bool eat(Entity cheese)
    {
        for (int i = 0; i < static_cast<int>(_active.size()); ++i) {
            if (_active.at(i) == cheese) {
                _active.erase(_active.begin() + i);
                _eaten.push_back(cheese);
                return true;
            }
        }
        return false;
    }

    // The cheeses eaten since the last call
    std::vector<Entity> takeEaten()
    {
        std::vector<Entity> eaten = _eaten;

        _eaten.clear();
        return eaten;
    }

  private:
    std::vector<Entity> _active;
    std::vector<Entity> _eaten;
};

#endif /* !CHEESES_HPP_ */
