#include "brkchrd/input_safety.hpp"

#include <algorithm>

namespace brkchrd {

void FacePressState::press(Face face) {
    const std::size_t index = static_cast<std::size_t>(face);
    if (index >= held_.size()) return;

    if (held_[index]) {
        const auto end = order_.begin() + static_cast<std::ptrdiff_t>(count_);
        const auto found = std::find(order_.begin(), end, face);
        if (found != end) {
            std::move(found + 1, end, found);
            order_[count_ - 1U] = face;
        }
        return;
    }

    held_[index] = true;
    if (count_ < order_.size()) order_[count_++] = face;
}

void FacePressState::release(Face face) {
    const std::size_t index = static_cast<std::size_t>(face);
    if (index >= held_.size()) return;
    held_[index] = false;

    const auto end = order_.begin() + static_cast<std::ptrdiff_t>(count_);
    const auto found = std::find(order_.begin(), end, face);
    if (found == end) return;

    std::move(found + 1, end, found);
    --count_;
}

void FacePressState::clear() {
    held_.fill(false);
    count_ = 0U;
}

bool FacePressState::held(Face face) const {
    const std::size_t index = static_cast<std::size_t>(face);
    return index < held_.size() && held_[index];
}

const std::array<bool, 4>& FacePressState::held_faces() const { return held_; }

std::optional<Face> FacePressState::active() const {
    if (count_ == 0U) return std::nullopt;
    return order_[count_ - 1U];
}

std::size_t FacePressState::count() const { return count_; }

bool trigger_hysteresis(bool currently_pressed,
                        std::int16_t value,
                        std::int16_t press_threshold,
                        std::int16_t release_threshold) {
    if (currently_pressed) return value >= release_threshold;
    return value > press_threshold;
}

} // namespace brkchrd
