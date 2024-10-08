#include "deleters.h"

#include <SDL_render.h>
#include <SDL_video.h>

void std::default_delete<SDL_Window>::operator()(SDL_Window* window) const noexcept
{
    SDL_DestroyWindow(window);
}

void std::default_delete<SDL_Renderer>::operator()(SDL_Renderer* renderer) const noexcept
{
    SDL_DestroyRenderer(renderer);
}

void std::default_delete<SDL_Texture>::operator()(SDL_Texture* p) const noexcept
{
    SDL_DestroyTexture(p);
}

void std::default_delete<SDL_Surface>::operator()(SDL_Surface* p) const noexcept
{
    SDL_FreeSurface(p);
}
