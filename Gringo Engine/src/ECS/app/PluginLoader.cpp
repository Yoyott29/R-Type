/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** PluginLoader
*/

#include "PluginLoader.hpp"
#include "EngineError.hpp"
#include "PluginApi.hpp"
#include <utility>

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace
{
#if defined(_WIN32)
constexpr char const *LIBRARY_SUFFIX = ".dll";

std::string lastError() { return "error code " + std::to_string(GetLastError()); }

void *openLibrary(std::filesystem::path const &path)
{
    // Searches the plugin's own folder for the libraries it depends on (another plugin)
    return LoadLibraryExW(path.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
}

void *findSymbol(void *library, char const *name)
{
    return reinterpret_cast<void *>(GetProcAddress(static_cast<HMODULE>(library), name));
}

std::filesystem::path executablePath()
{
    std::wstring buffer(MAX_PATH, L'\0');
    DWORD length = 0;

    while ((length = GetModuleFileNameW(nullptr, buffer.data(),
                                        static_cast<DWORD>(buffer.size()))) == buffer.size())
        buffer.resize(buffer.size() * 2);
    if (length == 0)
        throw EngineError("Could not find the executable path: " + lastError());
    buffer.resize(length);
    return buffer;
}
#else
constexpr char const *LIBRARY_SUFFIX = ".so";

std::string lastError()
{
    char const *error = dlerror();
    return error ? error : "unknown error";
}

void *openLibrary(std::filesystem::path const &path) { return dlopen(path.c_str(), RTLD_NOW); }

void *findSymbol(void *library, char const *name) { return dlsym(library, name); }

std::filesystem::path executablePath()
{
    std::error_code error;
    std::filesystem::path path = std::filesystem::read_symlink("/proc/self/exe", error);

    if (error)
        throw EngineError("Could not find the executable path: " + error.message());
    return path;
}
#endif
} // namespace

PluginLoader::PluginLoader(std::filesystem::path directory)
    : _directory(directory.empty() ? [&] {
          const std::filesystem::path executableDirectory = executablePath().parent_path();
          const std::filesystem::path localPlugins = executableDirectory / "plugins";
          const std::filesystem::path parentPlugins = executableDirectory.parent_path() / "plugins";
          const std::filesystem::path siblingEnginePlugins =
              executableDirectory / "Gringo Engine" / "plugins";
          const std::filesystem::path enginePlugins =
              executableDirectory.parent_path() / "Gringo Engine" / "plugins";
          if (std::filesystem::is_directory(localPlugins))
              return localPlugins;
          if (std::filesystem::is_directory(parentPlugins))
              return parentPlugins;
          if (std::filesystem::is_directory(siblingEnginePlugins))
              return siblingEnginePlugins;
          if (std::filesystem::is_directory(enginePlugins))
              return enginePlugins;
          return localPlugins;
      }()
                                   : std::move(directory))
{
}

std::shared_ptr<IPlugin> PluginLoader::load(std::string const &name) const
{
    std::filesystem::path const path =
        std::filesystem::absolute(_directory / (name + LIBRARY_SUFFIX));

    void *library = openLibrary(path);
    if (!library)
        throw PluginLoadError(name, lastError());

    auto create = reinterpret_cast<CreatePluginFunction>(findSymbol(library, PLUGIN_ENTRY_POINT));
    if (!create)
        throw PluginLoadError(name, "no " PLUGIN_ENTRY_POINT " entry point in " + path.string());
    std::shared_ptr<IPlugin> plugin(create());
    if (!plugin)
        throw PluginLoadError(name, PLUGIN_ENTRY_POINT " returned no plugin");
    return plugin;
}

std::vector<std::string> PluginLoader::listNames() const
{
    std::vector<std::string> names;
    std::error_code error;
    std::filesystem::directory_iterator iterator(_directory, error);

    // A missing plugins folder just means there is no plugin to list
    if (error)
        return names;

    for (auto const &entry : iterator) {
        if (entry.path().extension() == LIBRARY_SUFFIX)
            names.push_back(entry.path().stem().string());
    }
    return names;
}

bool PluginLoader::isAvailable(std::string const &name) const
{
    return std::filesystem::is_regular_file(_directory / (name + LIBRARY_SUFFIX));
}
