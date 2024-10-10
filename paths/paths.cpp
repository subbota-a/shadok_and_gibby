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
