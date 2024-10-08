#pragma once
#include "deleters.h"

#include <filesystem>

namespace render {

class Resources {
public:
    explicit Resources(std::string_view application_name);
    std::unique_ptr<SDL_Surface> loadImage(const std::string& file_name) const;
    std::unique_ptr<SDL_Texture> loadTexture(SDL_Renderer* renderer, const std::string& file_name) const;

private:
    std::filesystem::path assets_path_;
};

} // render
