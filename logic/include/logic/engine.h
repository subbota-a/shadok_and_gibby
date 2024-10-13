#pragma once

#include "domain/commands.h"
#include "domain/config.h"
#include "domain/state.h"

#include <random>

namespace logic {

namespace internal {
    class ObjectMap {
    public:
        enum class ObjectType : uint8_t { Empty, Player, Enemy, Flower };

        ObjectMap(int width, int height);

        void clean();
        domain::Position placeObject(ObjectType object);
        [[nodiscard]] ObjectType getType(domain::Position pos) const;
        void setType(domain::Position pos, ObjectType type);

    private:
        const int width_, height_;
        std::mt19937 rng_;
        std::vector<ObjectType> objects_bitmap_;
    };

    class ScoreGenerator {
    public:
        ScoreGenerator(unsigned min, unsigned max);
        unsigned generate();

    private:
        std::mt19937 rng_;
        std::uniform_int_distribution<unsigned> score_distribution_;
    };
} // namespace internal

class Engine {
public:
    explicit Engine(const domain::Config &config);
    void startGame();
    void move(const domain::Vector& direction);
    [[nodiscard]] const domain::State &getState() const { return state_; }

private:
    const domain::Config &config_;
    internal::ObjectMap objects_map_;
    internal::ScoreGenerator score_generator_;
    domain::State state_;

    void placeFlower(ptrdiff_t index);
    void moveEnemies();
    void movePlayer(const domain::Vector& direction);
    void movePlayerTo(const domain::Position& new_pos);
    void eatFlowerByPlayer();
    [[nodiscard]] ptrdiff_t getFlowerIndex(const domain::Position &pos) const;
    void updateStatusAfterPlayerHasMoved();
    void forwardEnemy(int enemy_index, const domain::Position& flower);
};

} // namespace logic
