#pragma once
#include "deleters.h"

#include <filesystem>

namespace render {

class Resources {
public:
    explicit Resources(std::string_view application_name);
    [[nodiscard]] std::unique_ptr<SDL_Surface> loadImage(const std::string& file_name) const;
    [[nodiscard]] std::unique_ptr<SDL_Texture> loadTexture(SDL_Renderer* renderer, const std::string& file_name) const;
    [[nodiscard]] std::unique_ptr<_TTF_Font>
    loadFontDPI(const std::string& file_name, int ptsize, unsigned hdpi, unsigned vdpi) const;
    [[nodiscard]] std::unique_ptr<_TTF_Font>
    loadFont(const std::string& file_name, int ptsize) const;

private:
    std::filesystem::path assets_path_;
};

} // namespace render
