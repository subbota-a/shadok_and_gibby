#include "ui/event_controller.h"

namespace ui {

class GameOverEventController : public EventController {
public:
    ~GameOverEventController() override = default;

    std::optional<Commands> HandleEvent(const SDL_Event& event) override
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

class Direction {
public:
    static const Direction Negative;
    static const Direction Positive;
    static const Direction Zero;

    bool operator==(const Direction&) const noexcept = default;
    bool operator!=(const Direction&) const noexcept = default;

    Direction() : d_{0} {}

    void apply(Direction dir)
    {
        if (dir != Zero) {
            *this = dir;
        }
    }

    [[nodiscard]] bool match(Direction dir) const {
        return dir == Zero || dir == *this;
    }

    [[nodiscard]] int toInt() const noexcept { return d_; }

private:
    int d_;
    constexpr explicit Direction(int direction) : d_{direction} {}
};

const Direction Direction::Negative{-1};
const Direction Direction::Positive{1};
const Direction Direction::Zero{0};

class PlayerTurnEventController : public EventController {
public:
    ~PlayerTurnEventController() override = default;

    std::optional<Commands> HandleEvent(const SDL_Event& event) override
    {
        if (auto res = game_controller_.HandleEvent(event)) {
            clear();
            return res;
        }
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            registerKeyDown(event.key.keysym.sym);
        } else if (event.type == SDL_KEYUP) {
            return handleKeyRelease(event.key.keysym.sym);
        }
        return std::nullopt;
    }

private:
    static std::unordered_map<SDL_Keycode, std::array<Direction, 2>> codes;
    GameOverEventController game_controller_;
    std::array<Direction, 2> directions_;
    int strokes_length = 0;
    int pressed_count = 0;

    void clear()
    {
        directions_.fill(Direction::Zero);
        strokes_length = 0;
        pressed_count = 0;
    }

    void registerKeyDown(SDL_Keycode keyCode)
    {
        if (const auto it = codes.find(keyCode); it != codes.end()){
            directions_[0].apply(it->second[0]);
            directions_[1].apply(it->second[1]);
            ++pressed_count;
            strokes_length = std::max(strokes_length, pressed_count);
        }
    }
    std::optional<Commands> handleKeyRelease(SDL_Keycode keyCode)
    {
        std::optional<Commands> result;
        if (const auto it = codes.find(keyCode); it != codes.end() && pressed_count > 0)
        {
            if (!directions_[0].match(it->second[0]) || !directions_[1].match(it->second[1])){
                --strokes_length;
            } else if (pressed_count == strokes_length){
                result = MoveCommand({directions_[0].toInt(), directions_[1].toInt()});
            }
            --pressed_count;
            if (pressed_count == 0){
                clear();
            }
        }
        return result;
    }
};

std::unordered_map<SDL_Keycode, std::array<Direction, 2>> PlayerTurnEventController::codes = {
    {SDLK_KP_7, {Direction::Negative, Direction::Positive}},
    {SDLK_KP_8, {Direction::Zero, Direction::Positive}},
    {SDLK_KP_9, {Direction::Positive, Direction::Positive}},
    {SDLK_KP_4, {Direction::Negative, Direction::Zero}},
    {SDLK_KP_6, {Direction::Positive, Direction::Zero}},
    {SDLK_KP_1, {Direction::Negative, Direction::Negative}},
    {SDLK_KP_2, {Direction::Zero, Direction::Negative}},
    {SDLK_KP_3, {Direction::Positive, Direction::Negative}},
    {SDLK_UP, {Direction::Zero, Direction::Positive}},
    {SDLK_LEFT, {Direction::Negative, Direction::Zero}},
    {SDLK_DOWN, {Direction::Zero, Direction::Negative}},
    {SDLK_RIGHT, {Direction::Positive, Direction::Zero}},
};

std::unique_ptr<EventController> createEventController(domain::GameStatus status)
{
    if (status == domain::GameStatus::PlayerTurn) {
        return std::make_unique<PlayerTurnEventController>();
    }
    return std::make_unique<GameOverEventController>();
}

} // namespace ui
