// Apart from ProjectBuilder.cpp: the editor's Structs.hpp and the engine's Application.hpp both
// define Entity and Prefab
#include "Application.hpp"
#include "PluginLoader.hpp"
#include "ProjectBuilder.hpp"
#include <memory>
#include <stdexcept>
#include <utility>

PluginTraits ProjectBuilder::inspectPlugins(const std::vector<std::string> &activePlugins)
{
    PluginTraits traits;
    std::string variantsOwner;
    const PluginLoader loader;
    for (const std::string &name : activePlugins) {
        const std::shared_ptr<IPlugin> plugin = loader.load(name);
        if (plugin->requiresRenderer())
            traits.needRenderer.insert(name);
        std::vector<BuildVariant> variants = plugin->buildVariants();
        if (variants.empty())
            continue;
        if (!variantsOwner.empty())
            throw std::runtime_error("Cannot build project: plugins " + variantsOwner + " and " +
                                     name + " both declare build variants");
        variantsOwner = name;
        traits.variants = std::move(variants);
    }
    return traits;
}
