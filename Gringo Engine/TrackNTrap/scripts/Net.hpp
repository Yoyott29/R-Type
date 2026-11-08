#ifndef NET_HPP_
#define NET_HPP_

#include "ActionInput.hpp"
#include "BuildVariantName.hpp"
#include "ByteStream.hpp"
#include "LocalPeer.hpp"
#include "NetEvents.hpp"
#include "Networked.hpp"
#include "Replication.hpp"
#include "Scene.hpp"
#include <cmath>
#include <cstdint>
#include <string>

#define ACTION_FORWARD 0
#define ACTION_BACK 1
#define ACTION_JUMP 2
#define ACTION_TRAP 3
#define ACTION_START 4

#define CAMERA_ANGLE_FIRST_BIT 16
#define CAMERA_ANGLE_BITS 9
#define CAMERA_ANGLE_STEPS 512

#define RAT_PREFAB_START "Rat"

struct StartRequest {
};

struct RatStatus {
    int lap = 1;
    int trapsLeft = 0;
    int hits = 0;
    int respawns = 0;
    bool boosted = false;
    bool moving = false;
    bool grounded = true;
};

#define RACE_LOBBY 0
#define RACE_COUNTDOWN 1
#define RACE_RUNNING 2
#define RACE_FINISHED 3

struct RaceStatus {
    int phase = RACE_LOBBY;
    float countdown = 0.0f;
    float time = 0.0f;
    int winner = 0;
    int laps = 3;
};

template <> struct NetCodec<RatStatus> {
    static void write(ByteWriter &writer, const RatStatus &status)
    {
        writer.write(static_cast<std::int32_t>(status.lap));
        writer.write(static_cast<std::int32_t>(status.trapsLeft));
        writer.write(static_cast<std::int32_t>(status.hits));
        writer.write(static_cast<std::int32_t>(status.respawns));
        writer.write(static_cast<std::uint8_t>(status.boosted));
        writer.write(static_cast<std::uint8_t>(status.moving));
        writer.write(static_cast<std::uint8_t>(status.grounded));
    }

    static RatStatus read(ByteReader &reader)
    {
        RatStatus status;
        status.lap = reader.read<std::int32_t>();
        status.trapsLeft = reader.read<std::int32_t>();
        status.hits = reader.read<std::int32_t>();
        status.respawns = reader.read<std::int32_t>();
        status.boosted = reader.read<std::uint8_t>() != 0;
        status.moving = reader.read<std::uint8_t>() != 0;
        status.grounded = reader.read<std::uint8_t>() != 0;
        return status;
    }
};

template <> struct NetCodec<RaceStatus> {
    static void write(ByteWriter &writer, const RaceStatus &status)
    {
        writer.write(static_cast<std::int32_t>(status.phase));
        writer.write(status.countdown);
        writer.write(status.time);
        writer.write(static_cast<std::int32_t>(status.winner));
        writer.write(static_cast<std::int32_t>(status.laps));
    }

    static RaceStatus read(ByteReader &reader)
    {
        RaceStatus status;
        status.phase = reader.read<std::int32_t>();
        status.countdown = reader.read<float>();
        status.time = reader.read<float>();
        status.winner = reader.read<std::int32_t>();
        status.laps = reader.read<std::int32_t>();
        return status;
    }
};

class Net
{
  public:
    static bool isClient(Scene &scene)
    {
        return scene.hasResource<BuildVariantName>() &&
               scene.getResource<BuildVariantName>().name == "client";
    }

    static bool isRat(Scene &scene, Entity entity)
    {
        if (!scene.hasComponent<Networked>(entity))
            return false;
        Networked &networked = scene.getComponent<Networked>(entity);
        return networked.owner != NO_PEER && networked.prefab.rfind(RAT_PREFAB_START, 0) == 0;
    }

    static int slot(Scene &scene, Entity rat)
    {
        std::string prefab = scene.getComponent<Networked>(rat).prefab;
        return std::stoi(prefab.substr(std::string(RAT_PREFAB_START).size()));
    }

    static PeerId hostPeer(Scene &scene)
    {
        PeerId host = NO_PEER;
        for (Entity entity : scene.view<Networked>()) {
            if (isRat(scene, entity) && scene.getComponent<Networked>(entity).owner < host)
                host = scene.getComponent<Networked>(entity).owner;
        }
        return host;
    }

    static PeerId myPeer(Scene &scene)
    {
        if (!scene.hasResource<LocalPeer>())
            return NO_PEER;
        return scene.getResource<LocalPeer>().id;
    }

    static bool findMyRat(Scene &scene, Entity &rat)
    {
        PeerId me = myPeer(scene);
        if (me == NO_PEER)
            return false;
        for (Entity entity : scene.view<Networked>()) {
            if (isRat(scene, entity) && scene.getComponent<Networked>(entity).owner == me) {
                rat = entity;
                return true;
            }
        }
        return false;
    }

    static void setCameraAngle(ActionInput &input, float degrees)
    {
        float turn = std::fmod(degrees, 360.0f);
        if (turn < 0.0f)
            turn += 360.0f;
        int step = static_cast<int>(turn / 360.0f * CAMERA_ANGLE_STEPS) % CAMERA_ANGLE_STEPS;

        for (int bit = 0; bit < CAMERA_ANGLE_BITS; ++bit)
            input.active.set(CAMERA_ANGLE_FIRST_BIT + bit, (step >> bit) & 1);
    }

    static float cameraAngle(const ActionInput &input)
    {
        int step = 0;

        for (int bit = 0; bit < CAMERA_ANGLE_BITS; ++bit) {
            if (input.active.test(CAMERA_ANGLE_FIRST_BIT + bit))
                step += 1 << bit;
        }
        return static_cast<float>(step) * 360.0f / CAMERA_ANGLE_STEPS;
    }

    static void replicateStatus(Scene &scene)
    {
        replicate<RatStatus>(scene, "RatStatus");
        replicate<RaceStatus>(scene, "RaceStatus");
    }

    static bool findRace(Scene &scene, RaceStatus &status)
    {
        for (Entity entity : scene.view<RaceStatus>()) {
            status = scene.getComponent<RaceStatus>(entity);
            return true;
        }
        return false;
    }
};

#endif /* !NET_HPP_ */
