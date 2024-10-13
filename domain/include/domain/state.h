#pragma once
#include "domain/units.h"

#include <Eigen/Core>
#include <vector>

namespace domain {

using Position = Eigen::Matrix<Scalar, 2, 1>;

struct Player {
    Position position;
    unsigned scores;
    unsigned steps;
};
struct Flowers {
    std::vector<Position> positions;
    std::vector<unsigned> scores;
};
struct Enemies {
    std::vector<Position> position;
};
enum class GameStatus : uint8_t { PlayerTurn, EnemiesTurn, PlayerWon, PlayerLost };

enum SoundEffects : uint8_t {
    None,
    PlayerMoved,
    PlayerCouldNotMove,
    PlayerAteFlower,
    GameStarted,
    PlayerWon,
    PlayerLost
};

struct State final {
    Player player;
    Enemies enemies;
    Flowers flowers;
    GameStatus game_status;
    SoundEffects sound_effects{None};
};

} // namespace domain
