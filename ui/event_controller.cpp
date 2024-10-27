#include "event_controller.h"

namespace ui {

class GameOverEventController : public EventController {
public:
    ~GameOverEventController() override = default;

    std::optional<Engine::Commands> HandleEvent(const SDL_Event& event) override
    {
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
            case SDLK_y:
                return StartCommand();
            case SDLK_n:
                return QuitCommand();
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
                return MoveCommand{.direction = {-1, 1}};
            case SDLK_KP_8:
                return MoveCommand{.direction = {0, 1}};
            case SDLK_KP_9:
                return MoveCommand{.direction = {1, 1}};
            case SDLK_KP_4:
                return MoveCommand{.direction = {-1, 0}};
            case SDLK_KP_6:
                return MoveCommand{.direction = {1, 0}};
            case SDLK_KP_1:
                return MoveCommand{.direction = {-1, -1}};
            case SDLK_KP_2:
                return MoveCommand{.direction = {0, -1}};
            case SDLK_KP_3:
                return MoveCommand{.direction = {1, -1}};
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
            return MoveCommand{.direction = {horizontal_movement_, vertical_movement_}};
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
