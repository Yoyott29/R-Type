/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Events
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"
#include <cstddef>
#include <utility>
#include <vector>

// Double buffered event queue, stored as a scene resource. An event lives for two updates (the
// frame it is sent in and the next one), so every reader running once per frame sees it once
template <typename Event> class Events
{
  public:
    void send(Event event) { _current.push_back({_nextId++, std::move(event)}); }

    // Drops the events of the previous frame, called once per frame by EventUpdateSystem
    void update()
    {
        _previous = std::move(_current);
        _current.clear();
    }

    // Calls callback on every event with an id >= cursor, then moves the cursor past them
    template <typename Callback> void read(std::size_t &cursor, Callback &&callback) const
    {
        for (auto const *buffer : {&_previous, &_current})
            for (Entry const &entry : *buffer)
                if (entry.id >= cursor) {
                    callback(entry.event);
                    cursor = entry.id + 1;
                }
    }

  private:
    struct Entry {
        std::size_t id;
        Event event;
    };

    std::vector<Entry> _previous;
    std::vector<Entry> _current;
    std::size_t _nextId = 0;
};

// Keeps the read position of one system, so each event is handled only once by that system
template <typename Event> class EventReader
{
  public:
    template <typename Callback> void read(Scene &scene, Callback &&callback)
    {
        scene.getResource<Events<Event>>().read(_cursor, std::forward<Callback>(callback));
    }

  private:
    std::size_t _cursor = 0;
};

template <typename Event> class EventUpdateSystem : public System
{
  public:
    void update(Scene &scene) override { scene.getResource<Events<Event>>().update(); }
};
