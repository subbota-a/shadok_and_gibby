#pragma once

#include <SDL_events.h>

#include "domain/state.h"
#include "ui/engine.h"

#include <memory>

namespace ui {

class EventController {
public:
    virtual ~EventController() = default;

    virtual std::optional<Engine::Commands> HandleEvent(const SDL_Event& event) = 0;
};

std::unique_ptr<EventController> createEventController(domain::GameStatus status);

} // namespace render
