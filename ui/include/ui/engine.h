#pragma once
#include "domain/config.h"
#include "domain/state.h"
#include "ui/commands.h"

#include <variant>

#include "commands.h"

namespace ui {
class Engine {
public:
    virtual ~Engine() = default;
    virtual void setConfig(const domain::Config &config) = 0;
    virtual void drawTransition(const domain::State &from_state, const domain::State &to_state) = 0;
    using Commands = std::variant<MoveCommand, QuitCommand, StartCommand>;
    [[nodiscard]] virtual Commands waitForPlayer(const domain::State& state) = 0;
};

} // namespace render
