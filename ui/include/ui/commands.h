#pragma once

#include "domain/units.h"

namespace ui {

struct MoveCommand {
    domain::Vector direction;
};

struct QuitCommand {};

struct StartCommand {};

} // namespace domain
