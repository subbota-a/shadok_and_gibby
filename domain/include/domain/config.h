#pragma once
#include "domain/units.h"

#include <Eigen/Core>

namespace domain {

using Size = Eigen::Array<Scalar, 2, 1>;

struct Config {
    Size field_size;
    unsigned number_of_enemies;
    unsigned number_of_flowers;
    std::pair<unsigned, unsigned> flower_scores_range;
    unsigned max_player_steps;
    unsigned min_player_scores;
};

} // namespace logic
