#include "BuildVariantName.hpp"
#include "Check.hpp"
#include "CommandLine.hpp"
#include "EngineError.hpp"
#include "NetworkPlugin.hpp"
#include "ProjectSettings.hpp"

static void fill(Scene &scene, std::string variant, std::vector<std::string> args,
                 std::string playerPrefabs = "")
{
    scene.addResource<CommandLine>(CommandLine{std::move(args)});
    scene.addResource<ProjectSettings>(ProjectSettings({{"Network",
                                                         {{"Host", "127.0.0.1"},
                                                          {"Port", "4242"},
                                                          {"Role", "Client"},
                                                          {"PlayerPrefabs", playerPrefabs}}}}));
    if (!variant.empty())
        scene.addResource<BuildVariantName>(BuildVariantName{std::move(variant)});
}

TEST_CASE(variantsAreServerThenClient)
{
    auto v = NetworkPlugin{}.buildVariants();
    CHECK(v.size() == 2 && v[0].name == "server" && v[0].headless && v[1].name == "client");
}

TEST_CASE(roleComesFromVariant)
{
    Scene server;
    fill(server, "server", {});
    CHECK(NetworkPlugin::resolveConfig(server).role == NetRole::Server);
    Scene client;
    fill(client, "client", {"10.0.0.2", "5000"});
    NetworkConfig config = NetworkPlugin::resolveConfig(client);
    CHECK(config.role == NetRole::Client && config.host == "10.0.0.2" && config.port == 5000);
    Scene bogus;
    fill(bogus, "bogus", {});
    CHECK_THROWS(NetworkPlugin::resolveConfig(bogus), EngineError);
}

TEST_CASE(playerPrefabsParsed)
{
    Scene scene;
    fill(scene, "server", {}, "Player, Player2,,");
    std::vector<std::string> const expected{"Player", "Player2"};
    CHECK(NetworkPlugin::resolveConfig(scene).playerPrefabs == expected);
}
