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
    virtual void drawTransition(const domain::State &from_state, const domain::State &to_state) = 0;
    [[nodiscard]] virtual std::variant<domain::MoveCommand, domain::QuitCommand, domain::StartCommand>
    waitForPlayer(const domain::State& state) = 0;
};

} // namespace render
