#pragma once
#include "domain/config.h"
#include "domain/state.h"

namespace ui {
class Engine {
public:
    virtual ~Engine() = default;
    virtual void setConfig(const domain::Config& config) = 0;
    virtual void monitorChanged() = 0;
    virtual void windowChanged() = 0;
    virtual void drawTransition(double fraction, const domain::State& from_state, const domain::State& to_state) const = 0;
    virtual void draw(const domain::State& state) const = 0;
    virtual void playSound(domain::SoundEffects effects) = 0;
};

} // namespace ui
