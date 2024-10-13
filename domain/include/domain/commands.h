#pragma once

#include "domain/state.h"

namespace domain {

struct MoveCommand {
    Vector direction;
};

struct QuitCommand {};

struct StartCommand {};

} // namespace domain
