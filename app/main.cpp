#include "logic/engine.h"
#include "render/sdl_engine_factory.h"

#include <variant>

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

int main()
{
    domain::Config config{
            .field_size = {20, 20},
            .player_step_size = 1,
            .enemy_step_size = 1,
            .number_of_enemies = 4,
            .number_of_flowers = 15,
            .flower_scores_range = {1, 10},
            .max_player_steps = 100,
            .min_player_scores = 100,
    };
    const auto render = render::create_sdl_engine();
    render->setConfig(config);
    logic::Engine logic(config);
    logic.startGame();

    for (bool quit = false; !quit;) {
        render->draw(logic.getState());
        auto command = render->getCommand(logic.getState());
        std::visit(
                overloaded{
                        [&](const domain::QuitCommand &) { quit = true; },
                        [&](const domain::MoveCommand &move) { logic.movePlayer(move.direction); },
                        [&](const domain::MoveEnemiesCommand &) { logic.moveEnemies(); },
                        [&](const domain::StartCommand &) { logic.startGame(); }},
                command);
    }
}
