/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** PluginLoader
*/

#include "PluginLoader.hpp"
#include "EngineError.hpp"
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

std::filesystem::path modulePath(HMODULE module)
{
    std::wstring buffer(MAX_PATH, L'\0');
    DWORD length = 0;

    while ((length = GetModuleFileNameW(module, buffer.data(), static_cast<DWORD>(buffer.size()))) ==
           buffer.size())
        buffer.resize(buffer.size() * 2);
    if (length == 0)
        throw EngineError("Could not find a module path: " + lastError());
    buffer.resize(length);
    return buffer;
}

// The file of the library a function's code is in
std::filesystem::path libraryOf(void *address)
{
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(address), &module))
        throw EngineError("Could not find the library of a plugin: " + lastError());
    return modulePath(module);
}

std::filesystem::path executablePath() { return modulePath(nullptr); }
#else
constexpr char const *LIBRARY_SUFFIX = ".so";

std::string lastError()
{
    char const *error = dlerror();
    return error ? error : "unknown error";
}

void *openLibrary(std::filesystem::path const &path) { return dlopen(path.c_str(), RTLD_NOW); }

// The file of the library a function's code is in
std::filesystem::path libraryOf(void *address)
{
    Dl_info info{};
    if (dladdr(address, &info) == 0 || !info.dli_fname)
        throw EngineError("Could not find the library of a plugin");
    return info.dli_fname;
}

std::filesystem::path executablePath()
{
    std::error_code error;
    std::filesystem::path path = std::filesystem::read_symlink("/proc/self/exe", error);

    if (error)
        throw EngineError("Could not find the executable path: " + error.message());
    return path;
}
#endif

std::filesystem::path libraryKey(std::filesystem::path const &path)
{
    return std::filesystem::weakly_canonical(std::filesystem::absolute(path));
}
} // namespace

PluginLoader::PluginLoader(std::string const &folder, std::filesystem::path directory)
    : _directory(directory.empty() ? [&] {
          const std::filesystem::path executableDirectory = executablePath().parent_path();
          const std::filesystem::path localPlugins = executableDirectory / folder;
          const std::filesystem::path parentPlugins = executableDirectory.parent_path() / folder;
          const std::filesystem::path siblingEnginePlugins =
              executableDirectory / "Gringo Engine" / folder;
          const std::filesystem::path enginePlugins =
              executableDirectory.parent_path() / "Gringo Engine" / folder;
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

std::map<std::filesystem::path, PluginLoader::PluginFactory> &PluginLoader::pluginFactories()
{
    static std::map<std::filesystem::path, PluginFactory> factories;
    return factories;
}

std::map<std::filesystem::path, PluginLoader::RendererFactory> &PluginLoader::rendererFactories()
{
    static std::map<std::filesystem::path, RendererFactory> factories;
    return factories;
}

bool PluginLoader::registerPlugin(PluginFactory create)
{
    pluginFactories()[libraryKey(libraryOf(reinterpret_cast<void *>(create)))] = create;
    return true;
}

bool PluginLoader::registerRenderer(RendererFactory create)
{
    rendererFactories()[libraryKey(libraryOf(reinterpret_cast<void *>(create)))] = create;
    return true;
}

std::filesystem::path PluginLoader::open(std::string const &name) const
{
    const std::filesystem::path path = _directory / (name + LIBRARY_SUFFIX);

    // Opening it runs its Plugin<T> / Renderer<T> registration (once, even if opened again)
    if (!openLibrary(std::filesystem::absolute(path)))
        throw PluginLoadError(name, lastError());
    return libraryKey(path);
}

std::shared_ptr<IPlugin> PluginLoader::load(std::string const &name) const
{
    const std::filesystem::path library = open(name);
    auto factory = pluginFactories().find(library);
    if (factory == pluginFactories().end())
        throw PluginLoadError(name, "no class deriving from Plugin<T> in " + library.string());
    return factory->second();
}

std::shared_ptr<IRenderer> PluginLoader::loadRenderer(std::string const &name) const
{
    const std::filesystem::path library = open(name);
    auto factory = rendererFactories().find(library);
    if (factory == rendererFactories().end())
        throw PluginLoadError(name, "no class deriving from Renderer<T> in " + library.string());
    return factory->second();
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
