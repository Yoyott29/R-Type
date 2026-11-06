/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ScheduleGraph
*/

#include "ScheduleGraph.hpp"
#include <array>
#include <map>
#include <optional>
#include <typeindex>

namespace
{
// Who touched a piece of data (a component or a resource) last
struct DataState {
    std::optional<std::size_t> lastWriter;
    std::vector<std::size_t> readersSinceWrite;
};

class GraphBuilder
{
  public:
    explicit GraphBuilder(ScheduleGraph &graph) : _graph(graph) {}

    void addExclusive(std::size_t node)
    {
        for (const std::size_t previous : _sinceExclusive)
            addEdge(previous, node);
        // Back-to-back exclusive systems: nothing sits between them, so chain them directly
        if (_sinceExclusive.empty() && _lastExclusive)
            addEdge(*_lastExclusive, node);
        _lastExclusive = node;
        _sinceExclusive.clear();
    }

    void addParallel(std::size_t node, const SystemAccess &access)
    {
        if (_lastExclusive)
            addEdge(*_lastExclusive, node);
        for (std::size_t type = 0; type < MAX_COMPONENTS; ++type)
            touch(_components[type], node, access.componentReads().test(type),
                  access.componentWrites().test(type));
        for (const std::type_index &resource : access.resourceReads())
            touch(_resources[resource], node, true, access.resourceWrites().count(resource) == 1);
        _sinceExclusive.push_back(node);
    }

  private:
    // A piece of data that is both read and written is handled as a write only, because the reads
    // include the writes
    void touch(DataState &data, std::size_t node, bool reads, bool writes)
    {
        if (!reads && !writes)
            return;
        if (data.lastWriter)
            addEdge(*data.lastWriter, node);
        if (!writes) {
            data.readersSinceWrite.push_back(node);
            return;
        }
        for (const std::size_t reader : data.readersSinceWrite)
            addEdge(reader, node);
        data.lastWriter = node;
        data.readersSinceWrite.clear();
    }

    // Every edge into `to` is added while `to` is the node being built, and nodes are built in
    // increasing order, so successor lists are sorted and a duplicate is always the last entry
    void addEdge(std::size_t from, std::size_t to)
    {
        std::vector<std::size_t> &successors = _graph.successors[from];
        if (!successors.empty() && successors.back() == to)
            return;
        successors.push_back(to);
        ++_graph.predecessorCount[to];
    }

    ScheduleGraph &_graph;
    std::array<DataState, MAX_COMPONENTS> _components = {};
    std::map<std::type_index, DataState> _resources;
    std::optional<std::size_t> _lastExclusive;
    std::vector<std::size_t> _sinceExclusive;
};

bool hasParallelism(const ScheduleGraph &graph)
{
    for (std::size_t i = 0; i + 1 < graph.systems.size(); ++i) {
        // Successor lists are sorted, so an edge i -> i + 1 is the first one
        const auto &next = graph.successors[i];
        if (next.empty() || next.front() != i + 1)
            return true;
    }
    return false;
}
} // namespace

ScheduleGraph buildScheduleGraph(std::vector<std::shared_ptr<System>> systems,
                                 const std::vector<const SystemAccess *> &accesses)
{
    ScheduleGraph graph;
    graph.systems = std::move(systems);
    const std::size_t count = graph.systems.size();
    graph.exclusive.assign(count, false);
    graph.successors.assign(count, {});
    graph.predecessorCount.assign(count, 0);

    GraphBuilder builder(graph);
    for (std::size_t node = 0; node < count; ++node) {
        if (accesses[node] == nullptr) {
            graph.exclusive[node] = true;
            builder.addExclusive(node);
        } else {
            builder.addParallel(node, *accesses[node]);
        }
    }
    graph.hasParallelism = hasParallelism(graph);
    return graph;
}
