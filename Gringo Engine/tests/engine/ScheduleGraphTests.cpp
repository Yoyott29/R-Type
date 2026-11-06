/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ScheduleGraphTests
*/

#include "Check.hpp"
#include "Scene.hpp"
#include "ScheduleGraph.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace
{
struct Position {
};
struct Speed {
};
struct Clock {
};
struct NoopSystem : System {
    void update(Scene &) override {}
};

enum Node : std::size_t { First, Second, Third, Fourth };

constexpr std::size_t NO_EDGE = 0;
constexpr std::size_t ONE_EDGE = 1;
constexpr std::size_t TWO_EDGES = 2;
constexpr std::size_t THREE_EDGES = 3;
constexpr std::size_t FOUR_SYSTEMS = 4;
constexpr std::size_t THREE_SYSTEMS = 3;

ScheduleGraph build(const std::vector<const SystemAccess *> &accesses)
{
    std::vector<std::shared_ptr<System>> systems;
    for (std::size_t i = 0; i < accesses.size(); ++i)
        systems.push_back(std::make_shared<NoopSystem>());
    return buildScheduleGraph(std::move(systems), accesses);
}

bool hasEdge(const ScheduleGraph &graph, std::size_t from, std::size_t to)
{
    const auto &next = graph.successors[from];
    return std::find(next.begin(), next.end(), to) != next.end();
}

std::size_t edgeCount(const ScheduleGraph &graph)
{
    std::size_t total = 0;
    for (const auto &next : graph.successors)
        total += next.size();
    return total;
}
} // namespace

TEST_CASE(writersChain)
{
    Scene scene;
    scene.registerComponent<Position>();
    const SystemAccess w = scene.access().write<Position>();
    const ScheduleGraph graph = build(std::vector<const SystemAccess *>(FOUR_SYSTEMS, &w));
    CHECK(hasEdge(graph, First, Second));
    CHECK(hasEdge(graph, Second, Third));
    CHECK(hasEdge(graph, Third, Fourth));
    CHECK(edgeCount(graph) == THREE_EDGES);
    CHECK(!graph.hasParallelism);
}

TEST_CASE(readersAreIndependent)
{
    Scene scene;
    scene.registerComponent<Position>();
    const SystemAccess r = scene.access().read<Position>();
    const ScheduleGraph graph = build(std::vector<const SystemAccess *>(THREE_SYSTEMS, &r));
    CHECK(edgeCount(graph) == NO_EDGE);
    CHECK(graph.hasParallelism);
}

TEST_CASE(readersThenWriter)
{
    Scene scene;
    scene.registerComponent<Position>();
    const SystemAccess r = scene.access().read<Position>();
    const SystemAccess w = scene.access().write<Position>();
    const ScheduleGraph graph = build({&r, &r, &w});
    CHECK(hasEdge(graph, First, Third));
    CHECK(hasEdge(graph, Second, Third));
    CHECK(edgeCount(graph) == TWO_EDGES);
    CHECK(graph.hasParallelism);
}

TEST_CASE(writerThenReaders)
{
    Scene scene;
    scene.registerComponent<Position>();
    const SystemAccess r = scene.access().read<Position>();
    const SystemAccess w = scene.access().write<Position>();
    const ScheduleGraph graph = build({&w, &r, &r});
    CHECK(hasEdge(graph, First, Second));
    CHECK(hasEdge(graph, First, Third));
    CHECK(edgeCount(graph) == TWO_EDGES);
    CHECK(graph.hasParallelism);
}

TEST_CASE(readWriteSameComponentIsOneEdge)
{
    Scene scene;
    scene.registerComponent<Position>();
    const SystemAccess both = scene.access().read<Position>().write<Position>();
    const SystemAccess w = scene.access().write<Position>();
    const ScheduleGraph graph = build({&both, &w});
    CHECK(hasEdge(graph, First, Second));
    CHECK(edgeCount(graph) == ONE_EDGE);
}

TEST_CASE(sharedDataGivesNoDuplicateEdge)
{
    Scene scene;
    scene.registerComponent<Position>();
    scene.registerComponent<Speed>();
    const SystemAccess both = scene.access().write<Position>().write<Speed>();
    const ScheduleGraph graph = build({&both, &both});
    CHECK(hasEdge(graph, First, Second));
    CHECK(edgeCount(graph) == ONE_EDGE);
}

TEST_CASE(resourcesConflictLikeComponents)
{
    Scene scene;
    const SystemAccess w = scene.access().writeResource<Clock>();
    const SystemAccess r = scene.access().readResource<Clock>();
    const ScheduleGraph graph = build({&w, &r});
    CHECK(hasEdge(graph, First, Second));
    CHECK(edgeCount(graph) == ONE_EDGE);
}

TEST_CASE(exclusiveIsBarrier)
{
    Scene scene;
    const SystemAccess none = scene.access();
    const ScheduleGraph graph = build({&none, &none, nullptr, &none});
    CHECK(hasEdge(graph, First, Third));
    CHECK(hasEdge(graph, Second, Third));
    CHECK(hasEdge(graph, Third, Fourth));
    CHECK(edgeCount(graph) == THREE_EDGES);
    CHECK((graph.exclusive == std::vector<bool>{false, false, true, false}));
    CHECK(graph.hasParallelism);
}

TEST_CASE(allExclusiveIsSequential)
{
    const ScheduleGraph graph = build({nullptr, nullptr, nullptr});
    CHECK(hasEdge(graph, First, Second));
    CHECK(hasEdge(graph, Second, Third));
    CHECK(edgeCount(graph) == TWO_EDGES);
    CHECK(!graph.hasParallelism);
}

TEST_CASE(predecessorCountMatchesEdges)
{
    Scene scene;
    scene.registerComponent<Position>();
    const SystemAccess r = scene.access().read<Position>();
    const SystemAccess w = scene.access().write<Position>();
    const ScheduleGraph graph = build({&r, &r, &w});
    CHECK((graph.predecessorCount == std::vector<std::size_t>{NO_EDGE, NO_EDGE, TWO_EDGES}));
}
