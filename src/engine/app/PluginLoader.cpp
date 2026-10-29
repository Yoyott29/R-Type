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

void closeLibrary(void *library) { FreeLibrary(static_cast<HMODULE>(library)); }

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

void closeLibrary(void *library) { dlclose(library); }

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

PluginLoader::PluginLoader(std::filesystem::path directory) : _directory(std::move(directory)) {}

PluginLoader::~PluginLoader()
{
    for (auto it = _libraries.rbegin(); it != _libraries.rend(); ++it)
        closeLibrary(*it);
}

std::shared_ptr<IPlugin> PluginLoader::load(std::string const &name)
{
    if (_directory.empty())
        _directory = executablePath().parent_path() / "plugins";
    std::filesystem::path const path =
        std::filesystem::absolute(_directory / (name + LIBRARY_SUFFIX));

    void *library = openLibrary(path);
    if (!library)
        throw PluginLoadError(name, lastError());
    _libraries.push_back(library);

    auto create = reinterpret_cast<CreatePluginFunction>(findSymbol(library, PLUGIN_ENTRY_POINT));
    if (!create)
        throw PluginLoadError(name, "no " PLUGIN_ENTRY_POINT " entry point in " + path.string());
    std::shared_ptr<IPlugin> plugin(create());
    if (!plugin)
        throw PluginLoadError(name, PLUGIN_ENTRY_POINT " returned no plugin");
    return plugin;
}
