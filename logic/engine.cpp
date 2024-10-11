#include "logic/engine.h"

#include <experimental/mdspan>
#include <iostream>
#include <unordered_set>

namespace logic {

using ObjectType = internal::ObjectMap::ObjectType;
using std::experimental::mdspan;

namespace {
    // according to Direction
    // NONE, UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT
    std::array<domain::Position, 9> directions = {{
            {0, 0}, // NONE
            {0, 1}, // UP
            {0, -1}, // DOWN
            {-1, 0}, // LEFT
            {1, 0}, // RIGHT
            {-1, 1}, // UP_LEFT
            {1, 1}, // UP_RIGHT
            {-1, -1}, // DOWN_LEFT
            {1, -1}, // DOWN_RIGHT
    }};
} // namespace

internal::ObjectMap::ObjectMap(int width, int height)
    : width_(width)
    , height_(height)
    , rng_(std::random_device()())
    , objects_bitmap_(width_ * height_, ObjectType::Empty)
{
}

void internal::ObjectMap::clean() { std::ranges::fill(objects_bitmap_, ObjectType::Empty); }

domain::Position internal::ObjectMap::placeObject(const ObjectType object)
{
    std::uniform_int_distribution<> x_gen(0, width_ - 1);
    std::uniform_int_distribution<> y_gen(0, height_ - 1);
    const auto objects = mdspan(objects_bitmap_.data(), width_, height_);
    for (;;) {
        auto x = x_gen(rng_);
        auto y = y_gen(rng_);
        if (objects(x, y) == ObjectType::Empty) {
            objects(x, y) = object;
            return {x, y};
        }
    }
}

ObjectType internal::ObjectMap::getType(domain::Position pos) const
{
    const auto objects = mdspan(objects_bitmap_.data(), width_, height_);
    return objects(pos[0], pos[1]);
}

void internal::ObjectMap::setType(domain::Position pos, ObjectType type)
{
    const auto objects = mdspan(objects_bitmap_.data(), width_, height_);
    objects(pos[0], pos[1]) = type;
}

internal::ScoreGenerator::ScoreGenerator(const unsigned min, const unsigned max)
    : rng_(std::random_device()())
    , score_distribution_{min, max}
{
}

unsigned internal::ScoreGenerator::generate() { return score_distribution_(rng_); }

Engine::Engine(const domain::Config &config)
    : config_(config)
    , objects_map_(config_.field_size[0], config_.field_size[1])
    , score_generator_(config_.flower_scores_range.first, config_.flower_scores_range.second)
{
    state_.enemies.position.resize(config_.number_of_enemies);
    state_.flowers.positions.resize(config_.number_of_flowers);
    state_.flowers.scores.resize(config_.number_of_flowers);
}

void Engine::startGame()
{
    objects_map_.clean();

    state_.player.scores = 0;
    state_.player.steps = 0;
    state_.player.position = objects_map_.placeObject(ObjectType::Player);

    std::ranges::generate(
            state_.enemies.position,
            std::bind_front(&internal::ObjectMap::placeObject, &objects_map_, ObjectType::Enemy));

    std::ranges::generate(
            state_.flowers.positions,
            std::bind_front(&internal::ObjectMap::placeObject, &objects_map_, ObjectType::Flower));
    std::ranges::generate(
            state_.flowers.scores,
            std::bind_front(&internal::ScoreGenerator::generate, &score_generator_));

    state_.game_status = domain::GameStatus::PlayerTurn;
}

void Engine::movePlayer(const domain::Direction &direction)
{
    const auto prev = state_.player.position;
    state_.player.position += directions[direction];
    state_.player.position[0] = std::clamp(state_.player.position[0], 0, config_.field_size[0] - 1);
    state_.player.position[1] = std::clamp(state_.player.position[1], 0, config_.field_size[1] - 1);
    if (state_.player.position == prev) {
        return;
    }

    switch (objects_map_.getType(state_.player.position)) {
    case ObjectType::Empty:
        objects_map_.setType(prev, ObjectType::Empty);
        objects_map_.setType(state_.player.position, ObjectType::Player);
        state_.player.steps++;
        break;
    case ObjectType::Player:
        std::cerr << "seconds player!\n";
        std::terminate();
        break;
    case ObjectType::Enemy:
        state_.player.position = prev; // beep!
        break;
    case ObjectType::Flower:
        objects_map_.setType(prev, ObjectType::Empty);
        objects_map_.setType(state_.player.position, ObjectType::Player);
        state_.player.steps++;
        eatFlowerByPlayer();
        break;
    }
    state_.game_status = getNextStatus();
}

void Engine::placeFlower(const ptrdiff_t index)
{
    state_.flowers.positions[index] = objects_map_.placeObject(ObjectType::Flower);
    state_.flowers.scores[index] = score_generator_.generate();
}

ptrdiff_t Engine::getFlowerIndex(const domain::Position &pos) const
{
    return std::distance(state_.flowers.positions.begin(), std::ranges::find(state_.flowers.positions, pos));
}

void Engine::eatFlowerByPlayer()
{
    const auto index = getFlowerIndex(state_.player.position);
    state_.player.scores += state_.flowers.scores[index];
    placeFlower(index);
}

domain::GameStatus Engine::getNextStatus() const
{
    if (state_.player.scores >= config_.min_player_scores) {
        return domain::GameStatus::PlayerWon;
    }
    if (state_.player.steps >= config_.max_player_steps) {
        return domain::GameStatus::PlayerLost;
    }
    return domain::GameStatus::EnemiesTurn;
}

void Engine::pointEnemy(const int enemy_index, const domain::Position &flower)
{
    auto& enemy = state_.enemies.position[enemy_index];
    domain::Position vec = flower - enemy;
    vec[0] = std::clamp(vec[0], -1, 1);
    vec[1] = std::clamp(vec[1], -1, 1);
    domain::Position new_pos = enemy + vec;
    auto place = objects_map_.getType(new_pos);
    if (place == ObjectType::Player || place == ObjectType::Enemy) {
        new_pos = enemy + domain::Position(0, vec[1]);
        place = objects_map_.getType(new_pos);
        if (place == ObjectType::Player || place == ObjectType::Enemy) {
            new_pos = enemy + domain::Position(vec[0], 0);
            place = objects_map_.getType(new_pos);
        }
        if (place == ObjectType::Player || place == ObjectType::Enemy) {
            return; // can't move enemy
        }
    }
    objects_map_.setType(enemy, ObjectType::Empty);
    objects_map_.setType(new_pos, ObjectType::Enemy);
    enemy = new_pos;
    if (place == ObjectType::Flower) {
        const auto flower_index = getFlowerIndex(new_pos);
        placeFlower(flower_index);
    }
}

std::vector<int> distanceBetween(const std::vector<domain::Position> &object, const domain::Position &pos)
{
    std::vector<int> result(object.size());
    std::ranges::transform(object, result.begin(), [pos](const domain::Position &p) -> int {
        return (pos - p).array().abs().maxCoeff();
    });
    return result;
}

void Engine::moveEnemies()
{
    state_.game_status = domain::GameStatus::PlayerTurn;

    const auto distance = distanceBetween(state_.flowers.positions, state_.player.position);
    std::vector<int> flowers(config_.number_of_flowers);
    std::iota(flowers.begin(), flowers.end(), 0);
    const auto flowers_to_handle = std::min(config_.number_of_enemies, config_.number_of_flowers);
    std::ranges::partial_sort(flowers, flowers.begin() + flowers_to_handle, [&](const int i1, const int i2) {
        return distance[i1] < distance[i2];
    });

    auto enemies = state_.enemies.position;
    std::vector<int> enemies_indexes(enemies.size());
    std::iota(enemies_indexes.begin(), enemies_indexes.end(), 0);

    for(unsigned i = 0; i < flowers_to_handle; i++) {
        const auto& flower_position = state_.flowers.positions[flowers[i]];
        const auto enemies_distance = distanceBetween(enemies, flower_position);
        const auto min_enemy = std::distance(enemies_distance.begin(), std::ranges::min_element(enemies_distance));

        pointEnemy(enemies_indexes[min_enemy], flower_position);

        enemies.erase(enemies.begin() + min_enemy);
        enemies_indexes.erase(enemies_indexes.begin() + min_enemy);
    }
}

} // namespace logic
