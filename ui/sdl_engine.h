#pragma once

#include "ui/engine.h"
#include "sdl_guard.h"
#include "surface.h"

#include <memory>

#include <magic_enum/magic_enum.hpp>

namespace ui {

class SdlEngine final : public Engine {
public:
    explicit SdlEngine();
    ~SdlEngine() override = default;

    void setConfig(const domain::Config& config) override;
    void draw(double fraction, const domain::State& from_state, const domain::State& to_state) const;
    void drawTransition(const domain::State& from_state, const domain::State& to_state) override;
    [[nodiscard]] Commands waitForPlayer(const domain::State& state) override;

private:
    SdlGuard sdl_library_;
    std::array<std::unique_ptr<Mix_Chunk>, magic_enum::enum_count<domain::SoundEffects>()> sounds_;

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

    void calcLayout();
    void drawField() const;
    void drawEnemies(double fraction, const domain::Enemies& from_enemies, const domain::Enemies& to_enemies) const;
    void drawPlayer(const double frac, const domain::Player& from_player, const domain::Player& to_player) const;
    [[nodiscard]] std::vector<Uint8> getFlowersColorMod(const std::vector<unsigned>& scores) const;
    void drawFlowers(double fraction, const domain::Flowers& from_flowers, const domain::Flowers& to_flowers) const;
    static SDL_Color getStatusColor(domain::GameStatus game_status);
    void drawStatus(double frac, const domain::State& from_state, const domain::State& to_state) const;
    void drawMessage(double frac, const domain::GameStatus& from_state, const domain::GameStatus& to_state) const;
    [[nodiscard]] std::vector<SDL_Rect> getTransitionCells(
            double fraction,
            const std::vector<domain::Position>& from_positions,
            const std::vector<domain::Position>& to_positions) const noexcept;
    SDL_Rect getTransitionCell(double frac, const domain::Position& from_position, const domain::Position& to_position) const noexcept;
    [[nodiscard]] std::vector<SDL_Rect> getCells(const std::vector<domain::Position>& positions) const noexcept;
    SDL_Rect getCell(const domain::Position& position) const noexcept;

    void reloadResources();
    void loadSounds();
};

} // namespace render
