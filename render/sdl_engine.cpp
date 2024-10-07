#include "sdl_engine.h"

#include "resources.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <chrono>
#include <iostream>
#include <ranges>
#include <thread>
#include <vector>

using namespace std::string_literals;

namespace render {

namespace {

    SDL_Rect getCell(const domain::Position& position, const int edge) noexcept
    {
        return {position[0] * edge, position[1] * edge, edge, edge};
    }

    SDL_Color getFlowerColor(const int score, const int min, const int range) noexcept
    {
        return {0, static_cast<Uint8>(55 + 200 * (score - min) / range), 0, 255};
    }

    int getFlowerAlpha(const int score, const int min, const int range) noexcept
    {
        return static_cast<Uint8>(55 + 200 * (score - min) / range);
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
}
void SdlGuard::deleter(SdlGuard*)
{
    IMG_Quit();
    SDL_Quit();
}

SdlEngine::SdlEngine() : cell_size_(0), out_height_(0)
{
    Resources resources(PROJECT_NAME);
    enemy_texture_ = surface_.CreateTextureFromSurface(resources.loadImage("enemy.png").get());
    shadok_texture_ = surface_.CreateTextureFromSurface(resources.loadImage("shadok.png").get());
    flower_texture_ = surface_.CreateTextureFromSurface(resources.loadImage("flower.png").get());
}

void SdlEngine::setConfig(const domain::Config& config) { config_ = config; }

void SdlEngine::draw(const domain::State& state)
{
    if (!config_) {
        throw std::logic_error("No config specified");
    }
    prepareDimensions();

    constexpr auto background_color = SDL_Color(0, 0, 0, 255);
    surface_.Clear(background_color);

    drawEnemies(state.enemies);
    drawPlayer(state.player);
    drawFlowers(state.flowers);
    drawStatus(state);

    surface_.Present();
}

std::variant<domain::MoveCommand, domain::MoveEnemiesCommand, domain::QuitCommand, domain::StartCommand>
SdlEngine::getCommand(const domain::GameStatus status)
{
    if (status == domain::GameStatus::EnemiesTurn) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return domain::MoveEnemiesCommand();
    }
    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return domain::QuitCommand();
        }
        if (event.type == SDL_KEYDOWN) {
            if (status == domain::GameStatus::PlayerLost || status == domain::GameStatus::PlayerWon) {
                switch (event.key.keysym.sym) {
                case SDLK_y:
                    return domain::StartCommand();
                case SDLK_n:
                    return domain::QuitCommand();
                }
            } else {
                switch (event.key.keysym.sym) {
                case SDLK_KP_7:
                    return domain::MoveCommand{.direction = domain::Direction::UP_LEFT};
                case SDLK_KP_8:
                    return domain::MoveCommand{.direction = domain::Direction::UP};
                case SDLK_KP_9:
                    return domain::MoveCommand{.direction = domain::Direction::UP_RIGHT};
                case SDLK_KP_4:
                    return domain::MoveCommand{.direction = domain::Direction::LEFT};
                case SDLK_KP_6:
                    return domain::MoveCommand{.direction = domain::Direction::RIGHT};
                case SDLK_KP_1:
                    return domain::MoveCommand{.direction = domain::Direction::DOWN_LEFT};
                case SDLK_KP_2:
                    return domain::MoveCommand{.direction = domain::Direction::DOWN};
                case SDLK_KP_3:
                    return domain::MoveCommand{.direction = domain::Direction::DOWN_RIGHT};
                }
            }
        }
    }
    return domain::QuitCommand();
}

void SdlEngine::prepareDimensions()
{
    const auto out = surface_.OutputSize();
    out_height_ = out.y;
    cell_size_ = (domain::Size(out.x, out.y) / config_->field_size).minCoeff();
}

std::vector<SDL_Rect> SdlEngine::getCells(const std::vector<domain::Position>& positions) const
{
    std::vector<SDL_Rect> cells(positions.size());
    std::ranges::transform(positions, cells.begin(), std::bind_back(&getCell, cell_size_));
    std::ranges::for_each(cells, [this](auto& cell) { cell.y = out_height_ - cell.y - cell.h; });
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
    constexpr auto color = SDL_Color(255, 0, 0, 255);
    for (auto& cell: getCells(enemies.position)) {
        surface_.DrawTexture(enemy_texture_.get(), nullptr, &cell);
    }
}
void SdlEngine::drawPlayer(const domain::Player& player) const
{
    constexpr auto color = SDL_Color(0, 0, 255, 255);
    auto cell = getCell(player.position, cell_size_);
    cell.y = out_height_ - cell.y - cell.h;

    int w, h;
    SDL_QueryTexture(shadok_texture_.get(), nullptr, nullptr, &w, &h);
    w = static_cast<int>(std::round(cell.h * static_cast<double>(w) / static_cast<double>(h)));
    cell.x += (cell.w - w) / 2;
    cell.w = w;
    surface_.DrawTexture(shadok_texture_.get(), nullptr, &cell);
}

void SdlEngine::drawFlowers(const domain::Flowers& flowers) const
{
    const auto rects = getCells(flowers.positions);
    const auto alphas = getFlowersAlpha(flowers.scores);
    std::ranges::for_each(std::ranges::views::zip(rects, alphas), [this](const auto& pair) {
        SDL_SetTextureAlphaMod(flower_texture_.get(), std::get<const Uint8&>(pair));
        surface_.DrawTexture(flower_texture_.get(), nullptr, &std::get<const SDL_Rect&>(pair));
        //surface_.FillRect(std::get<const SDL_Rect&>(pair), std::get<const SDL_Color&>(pair));
    });
}

void SdlEngine::drawStatus(const domain::State& state) const
{
    std::cout << "Scores: " << state.player.scores << ", steps: " << state.player.steps << std::endl;

    switch (state.game_status) {
    case domain::GameStatus::PlayerTurn:
        [[fallthrough]];
    case domain::GameStatus::EnemiesTurn:
        break;
    case domain::GameStatus::PlayerWon:
        std::cout << "Player Won!" << std::endl;
        break;
    case domain::GameStatus::PlayerLost:
        std::cout << "Player Lost!" << std::endl;
        break;
    }
}

} // namespace render
