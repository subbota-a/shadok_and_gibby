#include "logic/engine.h"
#include "paths/paths.h"
#include "render/sdl_engine_factory.h"

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>

#include <SDL_main.h>

#include <format>
#include <iostream>
#include <variant>

#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

#ifdef _WINDOWS
void showError(const char* message)
{
    MessageBoxA(nullptr, message, "Error", MB_OK | MB_ICONERROR);
}
#endif
#if defined(linux)
void showError(const char* message)
{
    std::cerr << message << std::endl;
}
#endif

std::filesystem::path getConfigPath()
{
    return paths::getAppConfigPath() / PROJECT_NAME ".toml";
}
domain::Config getConfig(const std::filesystem::path& config_filepath)
{
    domain::Config config{
            .field_size = {20, 20},
            .number_of_enemies = 4,
            .number_of_flowers = 15,
            .flower_scores_range = {5, 10},
            .max_player_steps = 100,
            .min_player_scores = 100,
    };
    if (exists(config_filepath)) {
        toml::parse_result result = toml::parse_file(config_filepath.c_str());
        if (!result) {
            showError(std::format(
                              "Configuration file '{}' parsing failed: {}",
                              config_filepath.string(),
                              result.error().description())
                              .c_str());
        } else {
            auto& table = result.table();
            config.field_size[0] = table["field_width"].value_or(config.field_size[0]);
            config.field_size[1] = table["field_height"].value_or(config.field_size[1]);
            config.number_of_enemies = table["number_of_enemies"].value_or(config.number_of_enemies);
            config.number_of_flowers = table["number_of_flowers"].value_or(config.number_of_flowers);
            config.flower_scores_range.first = table["flower_scores_min"].value_or(config.flower_scores_range.first);
            config.flower_scores_range.second = table["flower_scores_max"].value_or(config.flower_scores_range.second);
            config.max_player_steps = table["max_player_steps"].value_or(config.max_player_steps);
            config.min_player_scores = table["min_player_scores"].value_or(config.min_player_scores);
        }
    }
    return config;
}

bool validateConfig(const domain::Config& config)
{
    if (config.flower_scores_range.first >= config.flower_scores_range.second) {
        showError("Invalid flowers scores range, flower_scores_min < flower_scores_max expected.");
        return false;
    }
    return true;
}

void saveConfig(const domain::Config& config, const std::filesystem::path& path)
{
    auto tbl = toml::table{
            {"field_width", config.field_size[0]},
            {"field_height", config.field_size[1]},
            {"number_of_enemies", config.number_of_enemies},
            {"number_of_flowers", config.number_of_flowers},
            {"flower_scores_min", config.flower_scores_range.first},
            {"flower_scores_max", config.flower_scores_range.second},
            {"max_player_steps", config.max_player_steps},
            {"min_player_scores", config.min_player_scores},
    };
    std::ofstream out;
    out.open(path, std::ios::out);
    if (!out) {
        showError(std::format("Failed to create config file '{}'", path.string()).c_str());
    } else {
        out << "# Shadok and Gibby config\n\n";
        out << tbl << std::endl;
    }
}

int main(int, char**)
{
    try {
        const auto config = getConfig(getConfigPath());
        if (!validateConfig(config)) {
            return 1;
        }
        saveConfig(config, getConfigPath());
        const auto render = render::create_sdl_engine();
        render->setConfig(config);
        logic::Engine logic(config);
        logic.startGame();
        domain::State old_state = logic.getState();

        for (bool quit = false; !quit;) {
            render->drawTransition(old_state, logic.getState());
            auto command = render->waitForPlayer(logic.getState());
            old_state = logic.getState();
            std::visit(
                    overloaded{
                            [&](const domain::MoveCommand& move) { logic.move(move.direction); },
                            [&](const domain::QuitCommand&) { quit = true; },
                            [&](const domain::StartCommand&) { logic.startGame(); }},
                    command);
        }
    } catch (const std::exception& e) {
        showError(e.what());
        return 1;
    }
    return 0;
}
