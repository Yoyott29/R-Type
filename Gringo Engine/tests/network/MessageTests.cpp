#include "ByteStream.hpp"
#include "Check.hpp"
#include "Messages.hpp"
#include "Packet.hpp"
#include <numeric>

TEST_CASE(entityStateRoundTrip)
{
    EntityStateMessage const sent{12, 3, NO_PEER, "Player", {{0x1234, {std::byte{1}}}}};
    auto got = EntityStateMessage::decode(sent.encode());
    CHECK(got && got->tick == sent.tick && got->netId == sent.netId && got->prefab == sent.prefab &&
          got->blocks.size() == 1 && got->blocks[0].bytes == sent.blocks[0].bytes);
}

TEST_CASE(despawnsSplitToFit)
{
    std::vector<std::uint32_t> ids(700);
    std::iota(ids.begin(), ids.end(), 1u);
    std::size_t total = 0;
    for (auto const &part : DespawnMessage::encodeSplit(5, ids)) {
        CHECK(part.size() <= MAX_PAYLOAD);
        total += DespawnMessage::decode(part)->netIds.size();
    }
    CHECK(total == ids.size());
    CHECK(DespawnMessage::encodeSplit(5, {}).empty());
}

TEST_CASE(badEntityStatesRejected)
{
    CHECK_THROWS((EntityStateMessage{1, 1, 0, std::string(MAX_PREFAB_NAME + 1, 'x'), {}}.encode()),
                 NetError);
    auto bytes = EntityStateMessage{1, 1, 0, "P", {}}.encode();
    for (std::size_t cut = 0; cut < bytes.size(); ++cut)
        CHECK(!EntityStateMessage::decode(std::span(bytes).first(cut)));
    bytes.push_back(std::byte{0});
    CHECK(!EntityStateMessage::decode(bytes));
}
