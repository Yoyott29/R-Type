#ifndef TRAPS_HPP_
#define TRAPS_HPP_

#include "Entity.hpp"
#include <vector>

class Traps
{
  public:
    void add(Entity trap, Entity owner, float immunity, float life)
    {
        _flying.push_back({trap, owner, immunity, life});
    }

    bool covers(Entity rat, Entity trap)
    {
        for (Flying &entry : _flying) {
            if (entry.trap == trap && entry.owner == rat && entry.immunity > 0.0f)
                return true;
        }
        return false;
    }

    bool hit(Entity trap)
    {
        for (int i = 0; i < static_cast<int>(_flying.size()); ++i) {
            if (_flying.at(i).trap == trap) {
                _flying.erase(_flying.begin() + i);
                _done.push_back(trap);
                return true;
            }
        }
        return false;
    }

    void elapse(float seconds)
    {
        int i = 0;
        while (i < static_cast<int>(_flying.size())) {
            _flying.at(i).immunity -= seconds;
            _flying.at(i).life -= seconds;
            if (_flying.at(i).life <= 0.0f) {
                _done.push_back(_flying.at(i).trap);
                _flying.erase(_flying.begin() + i);
            } else {
                ++i;
            }
        }
    }

    std::vector<Entity> takeDone()
    {
        std::vector<Entity> done = _done;

        _done.clear();
        return done;
    }

  private:
    struct Flying {
        Entity trap;
        Entity owner;
        float immunity;
        float life;
    };

    std::vector<Flying> _flying;
    std::vector<Entity> _done;
};

#endif /* !TRAPS_HPP_ */
