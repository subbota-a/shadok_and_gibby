#pragma once

#include <SDL_events.h>

#include "domain/state.h"
#include "render/engine.h"

#include <memory>

namespace render {

class EventController {
public:
    virtual ~EventController() = default;

    virtual std::optional<Engine::Commands> HandleEvent(const SDL_Event& event) = 0;
};

std::unique_ptr<EventController> createEventController(domain::GameStatus status);

} // namespace render
