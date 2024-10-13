#pragma once
#include <memory>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Surface;
struct _TTF_Font;
typedef _TTF_Font TTF_Font;
struct Mix_Chunk;

template<>
struct std::default_delete<SDL_Window> {
    void operator()(SDL_Window* window) const noexcept;
};

template<>
struct std::default_delete<SDL_Renderer> {
    void operator()(SDL_Renderer* renderer) const noexcept;
};

template<>
struct std::default_delete<SDL_Texture> {
    void operator()(SDL_Texture* p) const noexcept;
};

template<>
struct std::default_delete<SDL_Surface> {
    void operator()(SDL_Surface* p) const noexcept;
};

template<>
struct std::default_delete<TTF_Font> {
    void operator()(TTF_Font* p) const noexcept;
};

template<>
struct std::default_delete<Mix_Chunk> {
    void operator()(Mix_Chunk* p) const noexcept;
};
