#pragma once

#include "render/engine.h"
#include "surface.h"

#include <memory>

namespace render {

class SdlGuard final {
public:
    explicit SdlGuard();

private:
    std::unique_ptr<SdlGuard, void (*)(SdlGuard*)> _impl;
    static void deleter(SdlGuard*);
};

class SdlEngine final : public Engine {
public:
    explicit SdlEngine();
    ~SdlEngine() override = default;

    void setConfig(const domain::Config& config) override;
    void draw(const domain::State& state) override;
    [[nodiscard]] std::variant<domain::MoveCommand, domain::MoveEnemiesCommand, domain::QuitCommand, domain::StartCommand>
            getCommand(const domain::State& state) override;

private:
    SdlGuard sdl_library_;
    Surface surface_;
    std::optional<domain::Config> config_;
    std::unique_ptr<_TTF_Font> large_font_;
    std::unique_ptr<_TTF_Font> small_font_;
    std::unique_ptr<SDL_Texture> enemy_texture_;
    std::unique_ptr<SDL_Texture> shadok_texture_;
    std::unique_ptr<SDL_Texture> flower_texture_;
    std::unique_ptr<SDL_Texture> grass_texture_;
    SDL_Point grass_size_;
    int font_size_{};
    SDL_Rect status_rect_{};
    SDL_Rect field_rect_{};
    int cell_size_{};

    void calcLayout(domain::GameStatus status);
    void drawField() const;
    void drawEnemies(const domain::Enemies& enemies) const;
    void drawPlayer(const domain::Player& player) const;
    [[nodiscard]] std::vector<Uint8> getFlowersAlpha(const std::vector<unsigned>& scores) const;
    void drawFlowers(const domain::Flowers& flowers) const;
    static SDL_Color getStatusColor(domain::GameStatus game_status);
    void drawStatus(const domain::State& state) const;
    void drawMessage(const domain::GameStatus& state) const;
    [[nodiscard]] std::vector<SDL_Rect> getCells(const std::vector<domain::Position>& positions) const;
    void reloadResources();
};

} // namespace render
