#pragma once
#include "domain/config.h"
#include "domain/state.h"

#include <variant>

#include "domain/commands.h"

namespace render {
class Engine {
public:
    virtual ~Engine() = default;
    virtual void setConfig(const domain::Config &config) = 0;
    virtual void draw(const domain::State &state) = 0;
    [[nodiscard]] virtual std::variant<domain::MoveCommand, domain::MoveEnemiesCommand, domain::QuitCommand, domain::StartCommand>
    getCommand(domain::GameStatus status) = 0;
};

} // namespace render
