#pragma once
#include <SDL_mixer.h>


#include "deleters.h"

#include <filesystem>

namespace ui {

class Resources {
public:
    explicit Resources(std::string_view application_name);
    [[nodiscard]] std::unique_ptr<SDL_Surface> loadImage(const std::string& file_name) const;
    [[nodiscard]] std::unique_ptr<SDL_Texture> loadTexture(SDL_Renderer* renderer, const std::string& file_name) const;
    [[nodiscard]] std::unique_ptr<TTF_Font>
    loadFontDPI(const std::string& file_name, int ptsize, unsigned hdpi, unsigned vdpi) const;
    [[nodiscard]] std::unique_ptr<TTF_Font>
    loadFont(const std::string& file_name, int ptsize) const;
    [[nodiscard]] std::unique_ptr<Mix_Chunk> loadSound(std::string_view file_name) const;

private:
    std::filesystem::path assets_path_;
};

} // namespace render
