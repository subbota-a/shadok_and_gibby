#include "resources.h"

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <ranges>
#include "paths/paths.h"

namespace render {
namespace {
    const char* constCharPtr(const std::u8string& s){
        return reinterpret_cast<const char*>(s.c_str());
    }
}
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
    auto texture = std::unique_ptr<SDL_Texture>(IMG_LoadTexture(renderer, constCharPtr((assets_path_ / file_name).u8string())));
    if (!texture) {
        throw std::runtime_error("IMG_Load failed!: "s + IMG_GetError());
    }
    return texture;
}

std::unique_ptr<TTF_Font>
Resources::loadFontDPI(const std::string& file_name, int ptsize, unsigned hdpi, unsigned vdpi) const
{
    using namespace std::string_literals;
    auto font = std::unique_ptr<TTF_Font>(TTF_OpenFontDPI(constCharPtr((assets_path_ / file_name).u8string()), ptsize, hdpi, vdpi));
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

} // namespace render
