#include "sdl_engine.h"

#include "resources.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <chrono>
#include <iostream>
#include <ranges>
#include <thread>
#include <vector>

#include <format>

using namespace std::string_literals;

namespace render {

namespace {

    SDL_Rect getCell(const domain::Position& position, const int edge, const SDL_Rect& field) noexcept
    {
        return {field.x + position[0] * edge, field.y + field.h - edge - position[1] * edge, edge, edge};
    }

    int getFlowerAlpha(const int score, const int min, const int range) noexcept
    {
        constexpr auto min_alpha = 100;
        return static_cast<Uint8>(min_alpha + (255 - min_alpha) * (score - min) / range);
    }

    enum Alignement : uint8_t {
        LEFT = 0x1,
        RIGHT = 0x2,
        CENTER_HOR = LEFT | RIGHT,
        TOP = 0x4,
        BOTTOM = 0x8,
        CENTER_VER = TOP | BOTTOM
    };

    SDL_Rect alignInRect(
            const int width,
            const int height,
            SDL_Rect frame,
            const uint8_t alignment,
            const int hor_padding,
            const int ver_padding) noexcept
    {
        frame.y += ver_padding;
        frame.h -= 2 * ver_padding;
        frame.x += hor_padding;
        frame.w -= 2 * hor_padding;
        SDL_Rect result{frame};
        if ((alignment & CENTER_HOR) == CENTER_HOR) {
            result.x += (frame.w - width) / 2;
            result.w = width;
        } else if (alignment & LEFT) {
            result.w = width;
        } else if (alignment & RIGHT) {
            result.x = frame.x + frame.w - width;
            result.w = width;
        }
        if ((alignment & CENTER_VER) == CENTER_VER) {
            result.y += (frame.h - height) / 2;
            result.h = height;
        } else if (alignment & TOP) {
            result.h = height;
        } else if (alignment & BOTTOM) {
            result.y = frame.y + frame.h - height;
            result.h = height;
        }
        return result;
    }

    SDL_Rect FitWindow(const int display, const SDL_Rect& window_bounds) noexcept
    {
        SDL_Rect usable_bounds;
        SDL_GetDisplayUsableBounds(display, &usable_bounds);
        SDL_Rect result;
        result.w = result.h = std::min(usable_bounds.w, usable_bounds.h) * 9 / 10;
        // result.x = window_bounds.x + std::min(usable_bounds.x + usable_bounds.w - window_bounds.x - result.w, 0);
        // result.y = window_bounds.y + std::min(usable_bounds.y + usable_bounds.h - window_bounds.y - result.h, 0);
        result.x = 0;
        result.y = 0;
        return result;
    }

} // namespace

SdlGuard::SdlGuard() : _impl(this, &SdlGuard::deleter)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
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
}
void SdlGuard::deleter(SdlGuard*)
{
    IMG_Quit();
    SDL_Quit();
}

SdlEngine::SdlEngine()
{
    reloadResources();
}

void SdlEngine::setConfig(const domain::Config& config)
{
    config_ = config;
}

void SdlEngine::draw(const domain::State& state)
{
    if (!config_) {
        throw std::logic_error("No config specified");
    }
    calcLayout(state.game_status);

    surface_.Clear(SDL_Color{50, 50, 50, 255});

    drawField();
    drawEnemies(state.enemies);
    drawPlayer(state.player);
    drawFlowers(state.flowers);
    drawStatus(state);
    drawMessage(state.game_status);

    surface_.Present();
}

std::variant<domain::MoveCommand, domain::MoveEnemiesCommand, domain::QuitCommand, domain::StartCommand>
SdlEngine::getCommand(const domain::State& state)
{
    if (state.game_status == domain::GameStatus::EnemiesTurn) {
        return domain::MoveEnemiesCommand();
    }
    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return domain::QuitCommand();
        }
        if (event.type == SDL_KEYDOWN) {
            if (state.game_status == domain::GameStatus::PlayerLost ||
                state.game_status == domain::GameStatus::PlayerWon) {
                switch (event.key.keysym.sym) {
                case SDLK_y:
                    return domain::StartCommand();
                case SDLK_n:
                    return domain::QuitCommand();
                }
            } else {
                switch (event.key.keysym.sym) {
                case SDLK_KP_7:
                case SDLK_w:
                    return domain::MoveCommand{.direction = domain::Direction::UP_LEFT};
                case SDLK_KP_8:
                case SDLK_e:
                    return domain::MoveCommand{.direction = domain::Direction::UP};
                case SDLK_KP_9:
                case SDLK_r:
                    return domain::MoveCommand{.direction = domain::Direction::UP_RIGHT};
                case SDLK_KP_4:
                case SDLK_s:
                    return domain::MoveCommand{.direction = domain::Direction::LEFT};
                case SDLK_KP_6:
                case SDLK_d:
                    return domain::MoveCommand{.direction = domain::Direction::RIGHT};
                case SDLK_KP_1:
                case SDLK_z:
                    return domain::MoveCommand{.direction = domain::Direction::DOWN_LEFT};
                case SDLK_KP_2:
                case SDLK_x:
                    return domain::MoveCommand{.direction = domain::Direction::DOWN};
                case SDLK_KP_3:
                case SDLK_c:
                    return domain::MoveCommand{.direction = domain::Direction::DOWN_RIGHT};
                }
            }
        }
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_EXPOSED || event.window.event == SDL_WINDOWEVENT_RESIZED ||
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_MOVED) {
                draw(state);
            }
            if (event.window.event == SDL_WINDOWEVENT_DISPLAY_CHANGED) {
                reloadResources();
            }
        }
    }
    return domain::QuitCommand();
}

void SdlEngine::reloadResources()
{
    float hdpi, vdpi;
    SDL_GetDisplayDPI(surface_.GetWindowDisplayIndex(), nullptr, &hdpi, &vdpi);
    const Resources resources(PROJECT_NAME);
    large_font_ = resources.loadFontDPI(
            "Days.otf",
            40,
            static_cast<unsigned>(std::round(hdpi)),
            static_cast<unsigned>(std::round(vdpi)));
    small_font_ = resources.loadFontDPI(
            "Days.otf",
            24,
            static_cast<unsigned>(std::round(hdpi)),
            static_cast<unsigned>(std::round(vdpi)));
    std::unique_ptr<SDL_Surface> text(
            TTF_RenderText_Solid(large_font_.get(), "Scores:", SDL_Color{255, 255, 255, 255}));
    font_size_ = text->h;
    enemy_texture_ = resources.loadTexture(surface_.Renderer(), "enemy.png");
    shadok_texture_ = resources.loadTexture(surface_.Renderer(), "shadok.png");
    flower_texture_ = resources.loadTexture(surface_.Renderer(), "flower.png");
    const auto grass_surface = resources.loadImage("grass.png");
    grass_texture_.reset(SDL_CreateTextureFromSurface(surface_.Renderer(), grass_surface.get()));
    grass_size_.x = grass_surface->w;
    grass_size_.y = grass_surface->h;
}

void SdlEngine::calcLayout(const domain::GameStatus status)
{
    const auto out = surface_.OutputSize();
    const int panel_height = 1.2 * font_size_;
    status_rect_ = SDL_Rect{0, 0, out.x, panel_height};
    field_rect_ = SDL_Rect{0, status_rect_.h, out.x, out.y - status_rect_.h};
    const int coeff = (domain::Size(field_rect_.w, field_rect_.h) / config_->field_size).minCoeff();
    field_rect_.x += (field_rect_.w - coeff * config_->field_size[0]) / 2;
    field_rect_.w = coeff * config_->field_size[0];
    // field_rect_.y += (field_rect_.h - coeff * config_->field_size[1]) / 2;
    field_rect_.h = coeff * config_->field_size[1];
    cell_size_ = field_rect_.w / config_->field_size[0];
}

void SdlEngine::drawField() const
{
    surface_.SetViewport(&field_rect_);
    SDL_SetTextureColorMod(grass_texture_.get(), 220, 220, 220);
    for (int y = 0; y < field_rect_.h; y += grass_size_.y) {
        for (int x = 0; x < field_rect_.w; x += grass_size_.x) {
            SDL_Rect target{x, y, grass_size_.x, grass_size_.y};
            surface_.DrawTexture(grass_texture_.get(), nullptr, &target);
        }
    }
    surface_.SetViewport(nullptr);
}

std::vector<SDL_Rect> SdlEngine::getCells(const std::vector<domain::Position>& positions) const
{
    std::vector<SDL_Rect> cells(positions.size());
    std::ranges::transform(positions, cells.begin(), std::bind_back(&getCell, cell_size_, field_rect_));
    return cells;
}

std::vector<Uint8> SdlEngine::getFlowersAlpha(const std::vector<unsigned>& scores) const
{
    std::vector<Uint8> alpha(scores.size());
    std::ranges::transform(
            scores,
            alpha.begin(),
            std::bind_back(
                    &getFlowerAlpha,
                    config_->flower_scores_range.first,
                    config_->flower_scores_range.second - config_->flower_scores_range.first));
    return alpha;
}

void SdlEngine::drawEnemies(const domain::Enemies& enemies) const
{
    for (auto& cell: getCells(enemies.position)) {
        surface_.DrawTexture(enemy_texture_.get(), nullptr, &cell);
    }
}

void SdlEngine::drawPlayer(const domain::Player& player) const
{
    const auto cell = getCell(player.position, cell_size_, field_rect_);
    surface_.DrawTexture(shadok_texture_.get(), nullptr, &cell);
}

void SdlEngine::drawFlowers(const domain::Flowers& flowers) const
{
    const auto rects = getCells(flowers.positions);
    const auto alphas = getFlowersAlpha(flowers.scores);
    std::ranges::for_each(
            std::ranges::views::zip(rects, alphas),
            [this](const std::tuple<const SDL_Rect&, const Uint8&>& pair) {
                const auto& [rect, coeff] = pair;
                SDL_SetTextureColorMod(flower_texture_.get(), coeff, coeff, coeff);
                surface_.DrawTexture(flower_texture_.get(), nullptr, &rect);
            });
}

SDL_Color SdlEngine::getStatusColor(const domain::GameStatus game_status)
{
    switch (game_status) {
    case domain::GameStatus::PlayerWon:
        return SDL_Color{0, 255, 0, 255};
    case domain::GameStatus::PlayerLost:
        return SDL_Color{255, 0, 0, 255};
    default:
        return SDL_Color{255, 255, 255, 255};
    }
}

void SdlEngine::drawStatus(const domain::State& state) const
{
    const auto status_text = std::format("Scores: {}, steps: {}", state.player.scores, state.player.steps);

    getStatusColor(state.game_status);
    std::unique_ptr<SDL_Surface> text_surface(
            TTF_RenderText_Solid(large_font_.get(), status_text.c_str(), getStatusColor(state.game_status)));
    std::unique_ptr<SDL_Texture> text_texture{SDL_CreateTextureFromSurface(surface_.Renderer(), text_surface.get())};
    SDL_Rect text_rect = alignInRect(
            text_surface->w,
            text_surface->h,
            status_rect_,
            Alignement::LEFT | Alignement::CENTER_VER,
            field_rect_.x,
            0);
    surface_.DrawTexture(text_texture.get(), nullptr, &text_rect);
}

void SdlEngine::drawMessage(const domain::GameStatus& state) const
{
    std::string message;
    switch (state) {
    case domain::GameStatus::PlayerWon:
        message = "You've won!";
        break;
    case domain::GameStatus::PlayerLost:
        message = "You've lost!";
        break;
    default:
        return;
    }
    std::unique_ptr<SDL_Surface> result_text_surface(
            TTF_RenderText_Solid(large_font_.get(), message.c_str(), SDL_Color{255, 255, 255, 255}));
    std::unique_ptr<SDL_Texture> result_text_texture{
            SDL_CreateTextureFromSurface(surface_.Renderer(), result_text_surface.get())};
    std::unique_ptr<SDL_Surface> prompt_text_surface(
            TTF_RenderText_Solid(small_font_.get(), "Continue, Y/N?", SDL_Color{252, 255, 51, 255}));
    std::unique_ptr<SDL_Texture> prompt_text_texture{
            SDL_CreateTextureFromSurface(surface_.Renderer(), prompt_text_surface.get())};
    const auto padding = result_text_surface->h / 8;
    const SDL_Rect panel_rect = alignInRect(
            std::max(result_text_surface->w, prompt_text_surface->w) + 4 * padding,
            result_text_surface->h + prompt_text_surface->h + 2 * padding,
            field_rect_,
            Alignement::CENTER_HOR | Alignement::CENTER_VER,
            0,
            0);
    surface_.FillRect(panel_rect, SDL_Color{255, 255, 255, 80});
    const SDL_Rect result_text_rect = alignInRect(
            result_text_surface->w,
            result_text_surface->h,
            panel_rect,
            Alignement::CENTER_HOR | Alignement::TOP,
            2 * padding,
            padding);
    surface_.DrawTexture(result_text_texture.get(), nullptr, &result_text_rect);
    const SDL_Rect prompt_text_rect = alignInRect(
            prompt_text_surface->w,
            prompt_text_surface->h,
            panel_rect,
            Alignement::CENTER_HOR | Alignement::BOTTOM,
            2 * padding,
            padding);
    surface_.DrawTexture(prompt_text_texture.get(), nullptr, &prompt_text_rect);
}

} // namespace render
