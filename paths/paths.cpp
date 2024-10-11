#include "paths/paths.h"
#if defined(linux)
#include <climits>
#include <unistd.h>
namespace paths {
std::filesystem::path getAppDataPath()
{
    // <executable file>/../share
    char result[PATH_MAX];
    const auto len = readlink("/proc/self/exe", result, sizeof(result));
    if (len == -1) {
        throw std::runtime_error("readlink failed");
    }
    return std::filesystem::path(std::string_view(result, len)).parent_path().parent_path() / "share";
}
std::filesystem::path getAppConfigPath()
{
    return std::filesystem::path(std::getenv("HOME")) / ".config";
}
} // namespace paths
#endif

#if defined _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace paths {
std::filesystem::path getAppDataPath()
{
    wchar_t result[MAX_PATH];

    const auto len = GetModuleFileNameW(nullptr, result, MAX_PATH);
    if (len == -1) {
        throw std::runtime_error("readlink failed");
    }
    return std::filesystem::path(std::wstring_view(result, len)).parent_path().parent_path();
}
std::filesystem::path getAppConfigPath()
{
    wchar_t result[MAX_PATH];
    size_t required;
    _wgetenv_s(&required, result, MAX_PATH, L"LOCALAPPDATA");
    return std::filesystem::path(std::wstring_view(result, required-1));
}
} // namespace paths
#endif
