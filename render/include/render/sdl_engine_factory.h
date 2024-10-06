#pragma once
#include "render/engine.h"
#include <memory>

namespace render{

std::unique_ptr<Engine> create_sdl_engine();

}
