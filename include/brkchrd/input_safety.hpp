#pragma once

#include "brkchrd/music.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace brkchrd {

class FacePressState {
public:
    void press(Face face);
    void release(Face face);
    void clear();

    bool held(Face face) const;
    const std::array<bool, 4>& held_faces() const;
    std::optional<Face> active() const;
    std::size_t count() const;

private:
    std::array<bool, 4> held_{};
    std::array<Face, 4> order_{};
    std::size_t count_ = 0U;
};

bool trigger_hysteresis(bool currently_pressed,
                        std::int16_t value,
                        std::int16_t press_threshold = 18000,
                        std::int16_t release_threshold = 12000);

} // namespace brkchrd
