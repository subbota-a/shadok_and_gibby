#include "event_controller.h"

namespace render {

class GameOverEventController : public EventController {
public:
    ~GameOverEventController() override = default;

    std::optional<Engine::Commands> HandleEvent(const SDL_Event& event) override
    {
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
            case SDLK_y:
                return domain::StartCommand();
            case SDLK_n:
                return domain::QuitCommand();
            }
        }
        return std::nullopt;
    }
};

class PlayerTurnEventController : public EventController {
public:
    ~PlayerTurnEventController() override = default;

    std::optional<Engine::Commands> HandleEvent(const SDL_Event& event) override
    {
        if (auto res = game_controller_.HandleEvent(event)) {
            return res;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
            case SDLK_KP_7:
                return domain::MoveCommand{.direction = {-1, 1}};
            case SDLK_KP_8:
                return domain::MoveCommand{.direction = {0, 1}};
            case SDLK_KP_9:
                return domain::MoveCommand{.direction = {1, 1}};
            case SDLK_KP_4:
                return domain::MoveCommand{.direction = {-1, 0}};
            case SDLK_KP_6:
                return domain::MoveCommand{.direction = {1, 0}};
            case SDLK_KP_1:
                return domain::MoveCommand{.direction = {-1, -1}};
            case SDLK_KP_2:
                return domain::MoveCommand{.direction = {0, -1}};
            case SDLK_KP_3:
                return domain::MoveCommand{.direction = {1, -1}};
            case SDLK_UP:
                vertical_movement_ = 1;
                break;
            case SDLK_LEFT:
                horizontal_movement_ = -1;
                break;
            case SDLK_DOWN:
                vertical_movement_ = -1;
                break;
            case SDLK_RIGHT:
                horizontal_movement_ = 1;
                break;
            default:
                vertical_movement_ = horizontal_movement_ = 0;
                break;
            }
        }
        if (event.type == SDL_KEYUP && (horizontal_movement_ | vertical_movement_)) {
            return domain::MoveCommand{.direction = {horizontal_movement_, vertical_movement_}};
        }
        return std::nullopt;
    }

private:
    GameOverEventController game_controller_;
    int horizontal_movement_ = 0;
    int vertical_movement_ = 0;
};

std::unique_ptr<EventController> createEventController(domain::GameStatus status)
{
    if (status == domain::GameStatus::PlayerTurn) {
        return std::make_unique<PlayerTurnEventController>();
    }
    return std::make_unique<GameOverEventController>();
}

} // namespace render
