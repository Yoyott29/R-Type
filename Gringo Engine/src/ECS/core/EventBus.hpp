/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EventBus
*/

#pragma once
#include <any>
#include <cstddef>
#include <functional>
#include <map>
#include <typeindex>
#include <unordered_map>

// Publish / subscribe, stored as a scene resource (added by the Application).
// emit(event) calls right away every callback subscribed to that event type
class EventBus
{
  public:
    template <typename Event> std::size_t subscribe(std::function<void(Event const &)> callback)
    {
        listeners<Event>()[_nextId] = std::move(callback);
        return _nextId++;
    }

    template <typename Event> void unsubscribe(std::size_t id) { listeners<Event>().erase(id); }

    template <typename Event> void emit(Event const &event)
    {
        // Copied, so a callback can subscribe or unsubscribe while the event is sent
        auto const callbacks = listeners<Event>();
        for (auto const &[id, callback] : callbacks)
            callback(event);
    }

  private:
    template <typename Event>
    using Callbacks = std::map<std::size_t, std::function<void(Event const &)>>;

    // The callbacks of one event type, created empty the first time it is used
    template <typename Event> Callbacks<Event> &listeners()
    {
        std::any &callbacks = _listeners[std::type_index(typeid(Event))];
        if (!callbacks.has_value())
            callbacks = Callbacks<Event>{};
        return std::any_cast<Callbacks<Event> &>(callbacks);
    }

    std::unordered_map<std::type_index, std::any> _listeners;
    std::size_t _nextId = 0;
};
