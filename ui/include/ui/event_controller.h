#pragma once

#include "SDL_events.h"

#include "domain/state.h"
#include "ui/commands.h"

#include <memory>
#include <variant>

namespace ui {

using Commands = std::variant<QuitCommand, MoveCommand, StartCommand>;

class EventController {
public:
    virtual ~EventController() = default;

    virtual std::optional<Commands> HandleEvent(const SDL_Event& event) = 0;
};

std::unique_ptr<EventController> createEventController(domain::GameStatus status);

} // namespace ui
