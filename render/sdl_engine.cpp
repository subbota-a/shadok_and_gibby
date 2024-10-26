#include "sdl_engine.h"

#include "resources.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <chrono>
#include <iostream>
#include <ranges>
#include <thread>
#include <vector>

#include <format>

using namespace std::string_literals;

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace render {

namespace {

    SDL_Rect getCell(const domain::Position& position, const int edge, const SDL_Rect& field) noexcept
    {
        return {field.x + position[0] * edge, field.y + field.h - edge - position[1] * edge, edge, edge};
    }

    int getFlowerColorMod(const int score, const int min, const int range) noexcept
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

    double easeInOut(const double speed)
    {
        return speed * speed * (3.0 - 2.0 * speed);
    }
    double getPlayerFrac(const double frac)
    {
        return easeInOut(std::min(frac * 1.20, 1.0));
    }

    double getEnemyFrac(const double frac)
    {
        return easeInOut(std::max(frac * 1.20 - 0.2, 0.0));
    }

    SDL_Rect scaleRect(const SDL_Rect& rect, const double scale)
    {
        SDL_Rect result;
        result.w = static_cast<int>(std::floor(scale * rect.w + 0.5));
        result.h = static_cast<int>(std::floor(scale * rect.h + 0.5));
        result.x = (rect.w - result.w) / 2 + rect.x;
        result.y = (rect.h - result.h) / 2 + rect.y;
        return result;
    }

} // namespace


SdlEngine::SdlEngine()
{
    loadSounds();
    reloadResources();
}

void SdlEngine::loadSounds()
{
    using namespace std::string_view_literals;
    // None,PlayerMoved,PlayerCouldNotMove,PlayerAteFlower,GameStarted,PlayerWon,PlayerLost
    std::array<std::string_view, magic_enum::enum_count<domain::SoundEffects>()> sounds_files =
            {""sv, "move.wav"sv, "error.wav"sv, "eat.mp3"sv, "start.mp3"sv, "win.mp3"sv, "lost.mp3"sv};
    const Resources resources(PROJECT_NAME);
    for (const auto index: magic_enum::enum_values<domain::SoundEffects>()) {
        if (const auto& name = sounds_files[index]; !name.empty()) {
            sounds_[index] = resources.loadSound(name);
        }
    }
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

void SdlEngine::setConfig(const domain::Config& config)
{
    config_ = config;
}

class GameOverEventController {
public:
    static std::optional<Engine::Commands> HandleEvent(const SDL_Event& event)
    {
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
            case SDLK_y:
                return domain::StartCommand();
            case SDLK_n:
                return domain::QuitCommand();
            }
        }
        return std::nullopt;
    }
};

class PlayerTurnEventController {
public:
    std::optional<Engine::Commands> HandleEvent(const SDL_Event& event)
    {
        if (auto res = game_controller_.HandleEvent(event)) {
            return res;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
            case SDLK_KP_7:
                return domain::MoveCommand{.direction = {-1, 1}};
            case SDLK_KP_8:
                return domain::MoveCommand{.direction = {0, 1}};
            case SDLK_KP_9:
                return domain::MoveCommand{.direction = {1, 1}};
            case SDLK_KP_4:
                return domain::MoveCommand{.direction = {-1, 0}};
            case SDLK_KP_6:
                return domain::MoveCommand{.direction = {1, 0}};
            case SDLK_KP_1:
                return domain::MoveCommand{.direction = {-1, -1}};
            case SDLK_KP_2:
                return domain::MoveCommand{.direction = {0, -1}};
            case SDLK_KP_3:
                return domain::MoveCommand{.direction = {1, -1}};
            case SDLK_UP:
                vertical_movement_ = 1;
                break;
            case SDLK_LEFT:
                horizontal_movement_ = -1;
                break;
            case SDLK_DOWN:
                vertical_movement_ = -1;
                break;
            case SDLK_RIGHT:
                horizontal_movement_ = 1;
                break;
            default:
                vertical_movement_ = horizontal_movement_ = 0;
                break;
            }
        }
        if (event.type == SDL_KEYUP && (horizontal_movement_ | vertical_movement_)) {
            return domain::MoveCommand{.direction = {horizontal_movement_, vertical_movement_}};
        }
        return std::nullopt;
    }

private:
    GameOverEventController game_controller_;
    int horizontal_movement_ = 0;
    int vertical_movement_ = 0;
};

Engine::Commands SdlEngine::waitForPlayer(const domain::State& state)
{
    assert(state.game_status != domain::GameStatus::EnemiesTurn);

    if (const auto& sound = sounds_[state.sound_effects]; sound) {
        Mix_PlayChannel(0, sound.get(), 0);
    }
    SDL_Event event;
    auto controller = [&]() -> std::variant<GameOverEventController, PlayerTurnEventController> {
        if (state.game_status == domain::GameStatus::PlayerTurn) {
            return PlayerTurnEventController{};
        }
        return GameOverEventController{};
    }();

    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return domain::QuitCommand();
        }
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_EXPOSED || event.window.event == SDL_WINDOWEVENT_RESIZED ||
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_MOVED) {
                draw(1.0, state, state);
            }
            if (event.window.event == SDL_WINDOWEVENT_DISPLAY_CHANGED) {
                reloadResources();
            }
        }
        if (auto result = std::visit([&](auto& c) { return c.HandleEvent(event); }, controller)) {
            return *result;
        }
    }
    return domain::QuitCommand();
}

void SdlEngine::drawTransition(const domain::State& from_state, const domain::State& to_state)
{
    if (!config_) {
        throw std::logic_error("No config specified");
    }
    calcLayout();

    SDL_DisplayMode display;
    SDL_GetDesktopDisplayMode(surface_.GetWindowDisplayIndex(), &display);
    constexpr auto transition_duration = std::chrono::duration<double>(0.4);
    const auto frame_duration = transition_duration / display.refresh_rate;
    const auto transition_start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::time_point{};
    for (;;) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return;
            }
        }
        now = std::chrono::steady_clock::now();
        const auto fraction = (now - transition_start) / transition_duration;
        if (fraction >= 1.0) {
            break;
        }
        draw(fraction, from_state, to_state);
    }
    draw(1.0, from_state, to_state);
}


void SdlEngine::draw(double fraction, const domain::State& from_state, const domain::State& to_state) const
{
    surface_.Clear(SDL_Color{50, 50, 50, 255});
    drawField();
    drawFlowers(fraction, from_state.flowers, to_state.flowers);
    drawEnemies(fraction, from_state.enemies, to_state.enemies);
    drawPlayer(fraction, from_state.player, to_state.player);
    drawStatus(fraction, from_state, to_state);
    drawMessage(fraction, from_state.game_status, to_state.game_status);
    surface_.Present();
}

void SdlEngine::calcLayout()
{
    const auto out = surface_.OutputSize();
    const int panel_height = 1.2 * font_size_;
    status_rect_ = SDL_Rect{0, 0, out.x, panel_height};
    field_rect_ = SDL_Rect{0, status_rect_.h, out.x, out.y - status_rect_.h};
    const int coeff = (Eigen::Array2i(field_rect_.w, field_rect_.h) / config_->field_size.cast<int>()).minCoeff();
    field_rect_.x += (field_rect_.w - coeff * config_->field_size[0]) / 2;
    field_rect_.w = coeff * config_->field_size[0];
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

SDL_Rect SdlEngine::getTransitionCell(
        const double frac, const domain::Position& from_position, const domain::Position& to_position) const noexcept
{
    return {
            .x = field_rect_.x +
                    static_cast<int>(
                            std::round((from_position[0] + (to_position[0] - from_position[0]) * frac) * cell_size_)),
            .y = field_rect_.y + field_rect_.h - cell_size_ -
                    static_cast<int>(
                            std::round((from_position[1] + (to_position[1] - from_position[1]) * frac) * cell_size_)),
            .w = cell_size_,
            .h = cell_size_,
    };
}

std::vector<SDL_Rect> SdlEngine::getTransitionCells(
        double fraction,
        const std::vector<domain::Position>& from_positions,
        const std::vector<domain::Position>& to_positions) const noexcept
{
    assert(from_positions.size() == to_positions.size());
    std::vector<SDL_Rect> cells(from_positions.size());
    std::ranges::transform(
            from_positions,
            to_positions,
            cells.begin(),
            std::bind_front(&SdlEngine::getTransitionCell, this, fraction));
    return cells;
}

SDL_Rect SdlEngine::getCell(const domain::Position& position) const noexcept
{
    return {
            .x = field_rect_.x + static_cast<int>(std::floor(position[0] * cell_size_ + 0.5)),
            .y = field_rect_.y + field_rect_.h - cell_size_ -
                    static_cast<int>(std::floor(position[1] * cell_size_ + 0.5)),
            .w = cell_size_,
            .h = cell_size_,
    };
}

std::vector<SDL_Rect> SdlEngine::getCells(const std::vector<domain::Position>& positions) const noexcept
{
    std::vector<SDL_Rect> cells(positions.size());
    std::ranges::transform(positions, cells.begin(), std::bind_front(&SdlEngine::getCell, this));
    return cells;
}

std::vector<Uint8> SdlEngine::getFlowersColorMod(const std::vector<unsigned>& scores) const
{
    std::vector<Uint8> alpha(scores.size());
    std::ranges::transform(
            scores,
            alpha.begin(),
            std::bind_back(
                    &getFlowerColorMod,
                    config_->flower_scores_range.first,
                    config_->flower_scores_range.second - config_->flower_scores_range.first));
    return alpha;
}

void SdlEngine::drawEnemies(
        const double fraction, const domain::Enemies& from_enemies, const domain::Enemies& to_enemies) const
{
    const auto f = getEnemyFrac(fraction);
    for (auto& cell: getTransitionCells(f, from_enemies.position, to_enemies.position)) {
        surface_.DrawTexture(enemy_texture_.get(), nullptr, &cell);
    }
}

void SdlEngine::drawPlayer(const double frac, const domain::Player& from_player, const domain::Player& to_player) const
{
    const auto cell = getTransitionCell(getPlayerFrac(frac), from_player.position, to_player.position);
    surface_.DrawTexture(shadok_texture_.get(), nullptr, &cell);
}

void SdlEngine::drawFlowers(
        double fraction, const domain::Flowers& from_flowers, const domain::Flowers& to_flowers) const
{
    if (fraction < 0.5) {
        const domain::Flowers& flowers = from_flowers;
        const auto rects = getCells(flowers.positions);
        const auto alphas = getFlowersColorMod(flowers.scores);
        std::ranges::for_each(
                std::ranges::views::zip(rects, alphas),
                [this](const std::tuple<const SDL_Rect&, const Uint8&>& pair) {
                    const auto& [rect, coeff] = pair;
                    SDL_SetTextureColorMod(flower_texture_.get(), coeff, coeff, coeff);
                    surface_.DrawTexture(flower_texture_.get(), nullptr, &rect);
                });
    } else {
        assert(from_flowers.positions.size() == to_flowers.positions.size());
        assert(from_flowers.scores.size() == to_flowers.scores.size());
        const auto from_rects = getCells(from_flowers.positions);
        const auto to_rects = getCells(to_flowers.positions);
        const auto from_alphas = getFlowersColorMod(from_flowers.scores);
        const auto to_alphas = getFlowersColorMod(to_flowers.scores);
        for (size_t i = 0; i < from_flowers.scores.size(); ++i) {
            SDL_SetTextureColorMod(flower_texture_.get(), from_alphas[i], from_alphas[i], from_alphas[i]);
            if (from_rects[i].x == to_rects[i].x && from_rects[i].y == to_rects[i].y) {
                surface_.DrawTexture(flower_texture_.get(), nullptr, &from_rects[i]);
            } else {
                const auto scale = (fraction - 0.5) * 2;
                const auto from_rect = scaleRect(from_rects[i], 1 - scale);
                surface_.DrawTexture(flower_texture_.get(), nullptr, &from_rect);

                SDL_SetTextureColorMod(flower_texture_.get(), to_alphas[i], to_alphas[i], to_alphas[i]);
                const auto to_rect = scaleRect(to_rects[i], scale);
                surface_.DrawTexture(flower_texture_.get(), nullptr, &to_rect);
            }
        }
    }
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

void SdlEngine::drawStatus(double frac, const domain::State& from_state, const domain::State& to_state) const
{
    const domain::State& state = frac < 1.0 ? from_state : to_state;
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

void SdlEngine::drawMessage(double frac, const domain::GameStatus& from_state, const domain::GameStatus& to_state) const
{
    const auto& state = frac < 1.0 ? from_state : to_state;
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
