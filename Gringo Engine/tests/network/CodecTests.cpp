#include "Check.hpp"
#include "Replication.hpp"

namespace
{
struct Speed {
    float x;
};
} // namespace

template <> struct NetCodec<Speed> {
    static void write(ByteWriter &writer, Speed const &value) { writer.write(value.x); }
    static Speed read(ByteReader &reader) { return Speed{reader.read<float>()}; }
};

TEST_CASE(codecRoundTrip)
{
    Scene scene;
    replicate<Position>(scene, "Position");
    Entity from = scene.createEntity();
    Entity to = scene.createEntity();
    Position const sent{1.5f, -2.0f};
    scene.addComponent(from, sent);
    CHECK(fnv1a32("") == 0x811C9DC5u && fnv1a32("a") == 0xE40C292Cu);
    auto &codec = scene.getResource<CodecRegistry>().codecs.at(fnv1a32("Position"));
    ByteWriter writer;
    codec.write(scene, from, writer);
    std::vector<std::byte> bytes = writer.take();

    std::vector<std::byte> const truncated(bytes.begin(), bytes.end() - 1);
    CHECK(!codec.apply(scene, to, truncated) && !codec.has(scene, to));
    CHECK(codec.apply(scene, to, bytes) && scene.getComponent<Position>(to).y == sent.y);
    codec.remove(scene, to);
    CHECK(!codec.has(scene, to));
}

TEST_CASE(nameClashThrows)
{
    Scene scene;
    replicate<Position>(scene, "Position");
    replicate<Position>(scene, "Position");
    CHECK_THROWS(replicate<Speed>(scene, "Position"), EngineError);
    CHECK(scene.getResource<CodecRegistry>().codecs.size() == 1);
}
