#include "brkchrd/music.hpp"
#include "brkchrd/synth.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

int main() {
    using namespace brkchrd;
    const auto power = make_chord(0, 0, Face::A, 0, 2, Direction::Up);
    assert(power.modifier == "POWER");
    assert((power.intervals == std::vector<int>{0,7,12}));
    const auto core = make_chord(0, 0, Face::B, 0, 0, Direction::Center);
    assert(core.name == "G");
    const auto voiced = voice_lead(core, {60,64,67});
    assert(!voiced.empty());

    SynthEngine synth(48000.0);
    assert(SynthEngine::preset_count() >= 10);
    assert(SynthEngine::effect_count() >= 8);
    const auto first = synth.preset_info();
    synth.set_preset(7);
    const auto doom = synth.preset_info();
    assert(first.name != doom.name);
    synth.set_parameter(SynthParameter::Tone, 0.9F);
    assert(std::abs(synth.parameter(SynthParameter::Tone) - 0.9F) < 0.001F);
    synth.set_effect(0, {EffectType::Crusher, 0.5F, 0.2F});
    assert(synth.effect(0).type == EffectType::Crusher);
    synth.play_chord({48,55,60});
    auto audio = synth.render_copy(48000);
    const float peak = *std::max_element(audio.begin(), audio.end(), [](float a, float b){ return std::abs(a) < std::abs(b); });
    assert(std::abs(peak) > 0.001F);
    assert(std::abs(peak) <= 1.001F);
    synth.release_chord();
    std::cout << "BRKCHRD tests passed\n";
    return 0;
}
