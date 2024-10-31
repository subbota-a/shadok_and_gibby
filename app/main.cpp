#include "config.h"
#include "logic/engine.h"
#include "ui/event_controller.h"
#include "ui/sdl_engine_factory.h"

#include <SDL_main.h>

#include <chrono>
#include <format>
#include <iostream>
#include <thread>
#include <variant>

#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef min
#undef max
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

struct VisualState {
    domain::GameStatus game_status;
    std::unique_ptr<ui::EventController> event_controller;
};

struct AnimationState : VisualState {
    static constexpr auto transition_duration = std::chrono::duration<double>(0.3);
    domain::State from_state;
    std::chrono::steady_clock::time_point transition_start = std::chrono::steady_clock::now();

    [[nodiscard]] double getFraction(const std::chrono::steady_clock::time_point now) const
    {
        return (now - transition_start) / transition_duration;
    }
};

struct PlayerTurnState : VisualState {};

struct MenuState : VisualState {};

using AppState = std::variant<MenuState, PlayerTurnState, AnimationState>;

domain::Config getConfig()
{
    if (exists(getConfigPath())) {
        if (auto res = loadConfig(getConfigPath())) {
            if (auto val = validateConfig(*res)) {
                return *res;
            } else {
                showError(val.error().c_str());
            }
        } else {
            showError(res.error().c_str());
        }
    }
    auto config = getDefaultConfig();
    if (auto save = saveConfig(config, getConfigPath()); !save) {
        showError(save.error().c_str());
    }
    return config;
}

std::optional<ui::Commands> nextCommand(ui::EventController& controller, ui::Engine& gui)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return ui::QuitCommand();
        }
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_DISPLAY_CHANGED) {
                gui.monitorChanged();
            }
            gui.windowChanged();
        }
        if (auto result = controller.HandleEvent(event)) {
            return *result;
        }
    }
    return std::nullopt;
}

ui::EventController& getEventController(AppState& state)
{
    return *std::visit([](auto& s) { return s.event_controller.get(); }, state);
}

VisualState createOrGetEventController(AppState& state, domain::GameStatus status)
{
    auto& visual_state = std::visit([](auto& s) -> VisualState& { return s; }, state);
    if (visual_state.game_status == status) {
        return std::move(visual_state);
    }
    return {
        .game_status = status,
        .event_controller = ui::createEventController(status),
    };
}

int main(int, char**)
{
    try {
        const auto config = getConfig();
        const auto gui = ui::create_sdl_engine();
        gui->setConfig(config);
        auto logic = std::make_unique<logic::Engine>(config);
        logic->startGame();
        gui->playSound(logic->getState().sound_effects);
        AppState app_state = PlayerTurnState{{
            .game_status = logic->getState().game_status,
            .event_controller = ui::createEventController(logic->getState().game_status),
        }};

        for (bool quit = false; !quit;) {
            auto command = nextCommand(getEventController(app_state), *gui);
            if (command) {
                std::visit(
                    overloaded{
                        [&](const ui::MoveCommand& move) {
                            const auto old_state = logic->getState();
                            logic->move(move.direction);
                            gui->playSound(logic->getState().sound_effects);
                            app_state = AnimationState{
                                {createOrGetEventController(app_state, logic->getState().game_status)},
                                old_state};
                        },
                        [&](const ui::QuitCommand&) { quit = true; },
                        [&](const ui::StartCommand&) {
                            logic->startGame();
                            gui->playSound(logic->getState().sound_effects);
                            app_state = PlayerTurnState{{
                                .game_status = logic->getState().game_status,
                                .event_controller = ui::createEventController(logic->getState().game_status),
                            }};
                        },
                    },
                    *command);
            }
            std::visit(
                overloaded{
                    [&](const AnimationState& animationState) {
                        const auto fraction =
                            std::min(animationState.getFraction(std::chrono::steady_clock::now()), 1.0);
                        gui->drawTransition(fraction, animationState.from_state, logic->getState());
                        if (fraction == 1.0) {
                            if (logic->getState().game_status == domain::GameStatus::PlayerTurn) {
                                app_state = PlayerTurnState{
                                    {createOrGetEventController(app_state, logic->getState().game_status)}};
                            } else {
                                app_state =
                                    MenuState{{createOrGetEventController(app_state, logic->getState().game_status)}};
                            }
                        }
                    },
                    [&](const PlayerTurnState&) {
                        gui->draw(logic->getState());
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    },
                    [&](const MenuState&) {
                        gui->draw(logic->getState());
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    }},
                app_state);
        }
    } catch (const std::exception& e) {
        showError(e.what());
        return 1;
    }
    return 0;
}
