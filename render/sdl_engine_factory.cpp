#include "render/sdl_engine_factory.h"

#include "sdl_engine.h"

namespace render {
std::unique_ptr<Engine> create_sdl_engine() {
    return std::make_unique<SdlEngine>();
}
}
