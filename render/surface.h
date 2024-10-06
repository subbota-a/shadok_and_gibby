#pragma once
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <memory>
#include <vector>

template<>
struct std::default_delete<SDL_Window> {
    void operator()(SDL_Window *window) const noexcept { SDL_DestroyWindow(window); }
};

template<>
struct std::default_delete<SDL_Renderer> {
    void operator()(SDL_Renderer *renderer) const noexcept { SDL_DestroyRenderer(renderer); }
};

namespace render {
class Surface {
public:
    Surface();
    void Clear(SDL_Color background) const;
    void Present() const;
    void DrawPolyline(const std::vector<SDL_Point> &polyline, SDL_Color color) const;
    void DrawLine(SDL_Point from, SDL_Point to, SDL_Color color) const;
    void FillRect(SDL_Rect rect, SDL_Color color) const;
    [[nodiscard]] SDL_Point OutputSize() const;
    [[nodiscard]] SDL_Point FromWindow(SDL_Point window_point) const;

private:
    std::unique_ptr<SDL_Window> window_;
    std::unique_ptr<SDL_Renderer> renderer_;

    void SetColor(SDL_Color color) const;
};

} // namespace render
