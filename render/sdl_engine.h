#pragma once

#include "render/engine.h"
#include "surface.h"

#include <memory>

namespace render {

class SdlGuard final {
public:
    explicit SdlGuard();

private:
    std::unique_ptr<SdlGuard, void (*)(SdlGuard *)> _impl;
    static void deleter(SdlGuard *);
};

class SdlEngine final : public Engine {
public:
    explicit SdlEngine() = default;
    ~SdlEngine() override = default;

    void setConfig(const domain::Config &config) override;
    void draw(const domain::State &state) override;
    [[nodiscard]] std::
            variant<domain::MoveCommand, domain::MoveEnemiesCommand, domain::QuitCommand, domain::StartCommand>
            getCommand(domain::GameStatus status) override;

private:
    SdlGuard sdl_library_;
    Surface surface_;
    std::optional<domain::Config> config_;
    int cell_size_;
    int out_height_;

    void PrepareDimensions();
    void drawEnemies(const domain::Enemies &enemies) const;
    void drawPlayer(const domain::Player &player) const;
    std::vector<SDL_Color> getFlowersColors(const std::vector<unsigned> &scores) const;
    void drawFlowers(const domain::Flowers &flowers) const;
    void drawStatus(const domain::State &state) const;
    std::vector<SDL_Rect> getCells(const std::vector<domain::Position> &positions) const;
};

} // namespace render
