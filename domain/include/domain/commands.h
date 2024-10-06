#pragma once

namespace domain {

enum Direction { NONE, UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT };

struct MoveCommand {
    Direction direction;
};

struct MoveEnemiesCommand{};

struct QuitCommand{};

struct StartCommand {};

}
