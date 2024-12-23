#include "sdl_guard.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <string_view>
#include <stdexcept>

using namespace std::string_literals;

namespace ui {

SdlGuard::SdlGuard() : _impl(this, &SdlGuard::deleter)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        throw std::runtime_error("SDL could not initialize! SDL_Error: "s + SDL_GetError());
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Quit();
        throw std::runtime_error("SDL image could not initialize! IMG_Error: "s + IMG_GetError());
    }
    if (TTF_Init() < 0) {
        IMG_Quit();
        SDL_Quit();
        throw std::runtime_error("SDL ttf could not initialize! TTF_Error: "s + TTF_GetError());
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0) {
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        throw std::runtime_error("SDL_mixer could not initialize! SDL_mixer Error: "s + Mix_GetError());
    }
}

void SdlGuard::deleter(SdlGuard*)
{
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

} // namespace render
