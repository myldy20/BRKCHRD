#include "brkchrd/music.hpp"
#include "brkchrd/synth.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <set>
#include <vector>

int main() {
    using namespace brkchrd;

    const auto power = make_chord(0, 0, Face::A, 0, 2, Direction::Up);
    assert(power.modifier == "POWER");
    assert((power.intervals == std::vector<int>{0,7,12}));

    const auto core = make_chord(0, 0, Face::B, 0, 0, Direction::Center);
    assert(core.name == "G");

    const auto minor_nine = make_chord(0, 0, Face::X, 0, 1, Direction::UpRight);
    assert(minor_nine.name == "Am9");
    assert((minor_nine.intervals == std::vector<int>{0,3,7,10,14}));

    for (int bank = 0; bank < 3; ++bank) {
        for (int palette = 0; palette < 3; ++palette) {
            for (int face = 0; face < 4; ++face) {
                for (int direction = 0; direction < 9; ++direction) {
                    const auto chord = make_chord(0, 0, static_cast<Face>(face), bank, palette, static_cast<Direction>(direction));
                    assert(!chord.intervals.empty());
                    assert(std::is_sorted(chord.intervals.begin(), chord.intervals.end()));
                    assert(std::set<int>(chord.intervals.begin(), chord.intervals.end()).size() == chord.intervals.size());
                    assert(chord.intervals.front() == 0);
                    assert(chord.intervals.back() <= 18);
                }
            }
        }
    }

    const auto stable_a = voice_chord(core, {}, false, VoicingProfile::Keys);
    const auto stable_b = voice_chord(core, {72,76,79}, false, VoicingProfile::Keys);
    assert(stable_a == stable_b);

    auto core_up = core;
    core_up.base_octave += 1;
    const auto octave_up = voice_chord(core_up, stable_a, false, VoicingProfile::Keys);
    assert(octave_up.size() == stable_a.size());
    for (std::size_t i = 0; i < stable_a.size(); ++i) assert(octave_up[i] == stable_a[i] + 12);

    const auto led = voice_chord(core, {60,64,67}, true, VoicingProfile::Keys);
    assert(!led.empty());
    const auto led_up = voice_chord(core_up, led, true, VoicingProfile::Keys);
    const double centre = static_cast<double>(std::accumulate(led_up.begin(), led_up.end(), 0)) / static_cast<double>(led_up.size());
    assert(centre > 66.0);

    const auto sub = voice_chord(make_chord(0, 0, Face::A, 0, 1, Direction::DownRight), {}, false, VoicingProfile::Bass);
    assert(sub.size() <= 3U);
    assert(sub.back() - sub.front() <= 12);

    SynthEngine synth(48000.0);
    assert(SynthEngine::preset_count() >= 16);
    assert(SynthEngine::effect_count() >= 8);
    const auto first = synth.preset_info();
    assert(synth.voicing_profile() == VoicingProfile::Keys);
    synth.set_preset(13);
    const auto bass = synth.preset_info();
    assert(first.name != bass.name);
    assert(synth.voicing_profile() == VoicingProfile::Bass);
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
