/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Replication
*/

#pragma once
#include "ByteStream.hpp"
#include "ChildOf.hpp"
#include "EngineError.hpp"
#include "Transform.hpp"
#include "Scene.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeindex>

using CodecId = std::uint32_t;

// 32-bit FNV-1a parameters
inline constexpr CodecId FNV1A32_OFFSET_BASIS = 2166136261u;
inline constexpr CodecId FNV1A32_PRIME = 16777619u;

constexpr CodecId fnv1a32(std::string_view text)
{
    CodecId hash = FNV1A32_OFFSET_BASIS;
    for (char c : text) {
        hash ^= static_cast<std::uint8_t>(c);
        hash *= FNV1A32_PRIME;
    }
    return hash;
}

// How a component goes on the wire: specialize with
//   static void write(ByteWriter &, T const &); static T read(ByteReader &);
template <typename T> struct NetCodec;

template <> struct NetCodec<Transform> {
    static void write(ByteWriter &writer, Transform const &value)
    {
        writer.write(value.x);
        writer.write(value.y);
        writer.write(value.z);
        writer.write(value.scaleX);
        writer.write(value.scaleY);
        writer.write(value.rotation);
    }

    static Transform read(ByteReader &reader)
    {
        Transform value{};
        value.x = reader.read<float>();
        value.y = reader.read<float>();
        value.z = reader.read<float>();
        value.scaleX = reader.read<float>();
        value.scaleY = reader.read<float>();
        value.rotation = reader.read<float>();
        return value;
    }
};

struct CodecEntry {
    std::string name;
    std::type_index type;
    std::function<bool(Scene &, Entity)> has;
    std::function<void(Scene &, Entity, ByteWriter &)> write;
    // Reads exactly one value, overwrites or adds it; false (entity untouched) on a short or long
    // read
    std::function<bool(Scene &, Entity, std::span<const std::byte>)> apply;
    std::function<void(Scene &, Entity)> remove;
};

// Scene resource: the replicated component types by CodecId
struct CodecRegistry {
    std::map<CodecId, CodecEntry> codecs;
};

// Registers T (idempotent) and its codec under name. The same (name, T) again does nothing;
// another type or name with the same id throws EngineError. ChildOf is never replicated
template <typename T> void replicate(Scene &scene, std::string_view name)
{
    if constexpr (std::is_same_v<T, ChildOf>)
        throw EngineError("ChildOf is never replicated");
    else {
        if (!scene.hasResource<CodecRegistry>())
            scene.addResource<CodecRegistry>();
        auto &codecs = scene.getResource<CodecRegistry>().codecs;
        CodecId const id = fnv1a32(name);
        auto const existing = codecs.find(id);
        if (existing != codecs.end()) {
            if (existing->second.name == name &&
                existing->second.type == std::type_index(typeid(T)))
                return;
            throw EngineError("Codec id clash: \"" + std::string(name) + "\" and \"" +
                              existing->second.name + "\"");
        }
        scene.registerComponent<T>();
        codecs.insert({id, CodecEntry{std::string(name), std::type_index(typeid(T)),
                                      [](Scene &s, Entity e) { return s.hasComponent<T>(e); },
                                      [](Scene &s, Entity e, ByteWriter &writer) {
                                          NetCodec<T>::write(writer, s.getComponent<T>(e));
                                      },
                                      [](Scene &s, Entity e, std::span<const std::byte> bytes) {
                                          try {
                                              ByteReader reader(bytes);
                                              T value = NetCodec<T>::read(reader);
                                              if (reader.remaining() != 0)
                                                  return false;
                                              if (s.hasComponent<T>(e))
                                                  s.getComponent<T>(e) = value;
                                              else
                                                  s.addComponent<T>(e, value);
                                              return true;
                                          } catch (NetError const &) {
                                              return false;
                                          }
                                      },
                                      [](Scene &s, Entity e) {
                                          if (s.hasComponent<T>(e))
                                              s.removeComponent<T>(e);
                                      }}});
    }
}
