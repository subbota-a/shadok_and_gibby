#pragma once
#include <SDL_surface.h>
#include <memory>
#include <filesystem>

template<>
struct std::default_delete<SDL_Surface> {
    void operator()(SDL_Surface* p) const noexcept { SDL_FreeSurface(p); }
};

namespace render {

class Resources {
public:
    explicit Resources(std::string_view application_name);
    std::unique_ptr<SDL_Surface> loadImage(const std::string& path) const;

private:
    std::filesystem::path assets_path_;
};

} // render
