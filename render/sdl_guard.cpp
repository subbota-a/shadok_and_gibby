#include "sdl_guard.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_mixer.h>
using namespace std::string_literals;

namespace render {

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
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
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
