#pragma once

#include <memory>

namespace ui {

class SdlGuard final {
public:
    explicit SdlGuard();

private:
    std::unique_ptr<SdlGuard, void (*)(SdlGuard*)> _impl;
    static void deleter(SdlGuard*);
};
} // namespace render
