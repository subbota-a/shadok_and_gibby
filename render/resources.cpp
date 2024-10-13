#include "resources.h"

#include "paths/paths.h"

#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <format>
#include <ranges>

namespace render {
namespace {
    const char* constCharPtr(const std::u8string& s)
    {
        return reinterpret_cast<const char*>(s.c_str());
    }
} // namespace
Resources::Resources(std::string_view application_name)
{
    assets_path_ = paths::getAppDataPath() / application_name / "assets";
    if (!exists(assets_path_)) {
        throw std::runtime_error("Could not find assets directory");
    }
}

std::unique_ptr<SDL_Surface> Resources::loadImage(const std::string& file_name) const
{
    using namespace std::string_literals;
    auto surface = std::unique_ptr<SDL_Surface>(IMG_Load(constCharPtr((assets_path_ / file_name).u8string())));
    if (!surface) {
        throw std::runtime_error("IMG_Load failed!: "s + IMG_GetError());
    }
    return surface;
}

std::unique_ptr<SDL_Texture> Resources::loadTexture(SDL_Renderer* renderer, const std::string& file_name) const
{
    using namespace std::string_literals;
    auto texture = std::unique_ptr<SDL_Texture>(
            IMG_LoadTexture(renderer, constCharPtr((assets_path_ / file_name).u8string())));
    if (!texture) {
        throw std::runtime_error("IMG_Load failed!: "s + IMG_GetError());
    }
    return texture;
}

std::unique_ptr<TTF_Font>
Resources::loadFontDPI(const std::string& file_name, int ptsize, unsigned hdpi, unsigned vdpi) const
{
    using namespace std::string_literals;
    auto font = std::unique_ptr<TTF_Font>(
            TTF_OpenFontDPI(constCharPtr((assets_path_ / file_name).u8string()), ptsize, hdpi, vdpi));
    if (!font) {
        throw std::runtime_error("TTF_OpenFontDPI failed!: "s + TTF_GetError());
    }
    return font;
}

std::unique_ptr<TTF_Font> Resources::loadFont(const std::string& file_name, int ptsize) const
{
    using namespace std::string_literals;
    auto font = std::unique_ptr<TTF_Font>(TTF_OpenFont(constCharPtr((assets_path_ / file_name).u8string()), ptsize));
    if (!font) {
        throw std::runtime_error("TTF_OpenFontDPI failed!: "s + TTF_GetError());
    }
    return font;
}

std::unique_ptr<Mix_Chunk> Resources::loadSound(std::string_view file_name) const
{
    using namespace std::string_literals;
    auto sound = std::unique_ptr<Mix_Chunk>(Mix_LoadWAV(constCharPtr((assets_path_ / file_name).u8string())));
    if (!sound) {
        throw std::runtime_error(
                std::format("Mix_LoadWAV({}) failed!: {}", (assets_path_ / file_name).string(), Mix_GetError()));
    }
    return sound;
}

} // namespace render
