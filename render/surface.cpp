#include "surface.h"

#include <cmath>
#include <stdexcept>

namespace render {

Surface::Surface()
    : window_(SDL_CreateWindow(
              "Шадок и Гибби",
              SDL_WINDOWPOS_UNDEFINED,
              SDL_WINDOWPOS_UNDEFINED,
              900,
              900,
              SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MAXIMIZED))
{
    if (!window_) {
        throw std::runtime_error(SDL_GetError());
    }
    renderer_.reset(SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    if (!renderer_) {
        throw std::runtime_error(SDL_GetError());
    }
}

int Surface::GetWindowDisplayIndex() const
{
    return SDL_GetWindowDisplayIndex(window_.get());
}

void Surface::ResizeWindow(const int width, const int height) const
{
    SDL_SetWindowSize(window_.get(), width, height);
}

void Surface::Clear(SDL_Color background) const
{
    SetColor(background);
    SDL_RenderClear(renderer_.get());
    SDL_SetRenderDrawBlendMode(renderer_.get(), SDL_BLENDMODE_NONE);
}

void Surface::Present() const
{
    SDL_RenderPresent(renderer_.get());
}

void Surface::DrawPolyline(const std::vector<SDL_Point>& polyline, SDL_Color color) const
{
    SetColor(color);
    SDL_RenderDrawLines(renderer_.get(), polyline.data(), static_cast<int>(polyline.size()));
}
void Surface::DrawLine(SDL_Point from, SDL_Point to, SDL_Color color) const
{
    SetColor(color);
    SDL_RenderDrawLine(renderer_.get(), from.x, from.y, to.x, to.y);
}
void Surface::FillRect(const SDL_Rect rect, SDL_Color color) const
{
    SDL_SetRenderDrawBlendMode(renderer_.get(), SDL_BLENDMODE_BLEND);
    SetColor(color);
    SDL_RenderFillRect(renderer_.get(), &rect);
}

void Surface::DrawTexture(const SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dest) const
{
    SDL_RenderCopy(renderer_.get(), const_cast<SDL_Texture*>(texture), src, dest);
}

SDL_Point Surface::OutputSize() const
{
    SDL_Point result;
    SDL_GetRendererOutputSize(renderer_.get(), &result.x, &result.y);
    return result;
}
SDL_Point Surface::FromWindow(SDL_Point window_point) const
{
    float x, y;
    SDL_RenderWindowToLogical(renderer_.get(), window_point.x, window_point.y, &x, &y);
    return SDL_Point{
            .x = static_cast<int>(std::round(x)),
            .y = static_cast<int>(std::round(y)),
    };
}

void Surface::SetColor(SDL_Color color) const
{
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
}

}
