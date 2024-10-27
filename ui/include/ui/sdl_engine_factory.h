#pragma once
#include "ui/engine.h"
#include <memory>

namespace ui {

std::unique_ptr<Engine> create_sdl_engine();

}
