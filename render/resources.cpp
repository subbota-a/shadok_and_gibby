#include "resources.h"

#include <SDL_image.h>
#include <ranges>

namespace render {

Resources::Resources(std::string_view application_name)
{
#ifdef __LINUX__
    const auto* dirs = std::getenv("HOME");
    char delimiter = ':';
    for (auto dir: std::views::split(std::string_view(dirs), delimiter)) {
        const auto tmp = std::filesystem::path(std::string_view(dir)) / ".local/share" / application_name / "assets";
        if (exists(tmp)) {
            assets_path_ = tmp;
            break;
        }
    }
    if (assets_path_.empty()) {
        throw std::runtime_error("Could not find assets directory");
    }
#endif
}

std::unique_ptr<SDL_Surface> Resources::loadImage(const std::string& path) const
{
    using namespace std::string_literals;
    auto surface = std::unique_ptr<SDL_Surface>(IMG_Load((assets_path_ / path).c_str()));
    if (!surface) {
        throw std::runtime_error("IMG_Load failed!: "s + IMG_GetError());
    }
    return surface;
}

} // namespace render
