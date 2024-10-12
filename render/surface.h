#pragma once
#include "deleters.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <vector>

namespace render {
class Surface {
public:
    Surface();
    [[nodiscard]] int GetWindowDisplayIndex() const;
    void ResizeWindow(int width, int height) const;
    void RepositionWindow(int x, int y) const;
    [[nodiscard]] SDL_Rect GetWindowRect() const;
    void Clear(SDL_Color background) const;
    void Present() const;
    void DrawPolyline(const std::vector<SDL_Point>& polyline, SDL_Color color) const;
    void DrawLine(SDL_Point from, SDL_Point to, SDL_Color color) const;
    void FillRect(SDL_Rect rect, SDL_Color color) const;
    void DrawTexture(const SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dest) const;
    [[nodiscard]] SDL_Point OutputSize() const;
    [[nodiscard]] SDL_Point FromWindow(SDL_Point window_point) const;
    [[nodiscard]] SDL_Renderer* Renderer() const { return renderer_.get(); }
    void SetViewport(const SDL_Rect* clip_rect) const;

private:
    std::unique_ptr<SDL_Window> window_;
    std::unique_ptr<SDL_Renderer> renderer_;

    void SetColor(SDL_Color color) const;
};

} // namespace render
