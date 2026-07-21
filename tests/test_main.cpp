#include "brkchrd/config.hpp"
#include "brkchrd/input_safety.hpp"
#include "brkchrd/music.hpp"
#include "brkchrd/synth.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>

namespace {

int failures = 0;

void check(bool condition, const char* expression, const char* file, int line) {
    if (condition) return;
    std::cerr << file << ':' << line << ": check failed: " << expression << '\n';
    ++failures;
}

#define CHECK(condition) check(static_cast<bool>(condition), #condition, __FILE__, __LINE__)

float absolute_peak(const std::vector<float>& audio) {
    float peak = 0.0F;
    for (float sample : audio) peak = std::max(peak, std::abs(sample));
    return peak;
}

} // namespace

int main() {
    using namespace brkchrd;

    const auto power = make_chord(0, 0, Face::A, 0, 2, Direction::Up);
    const std::vector<int> expected_power{0, 7, 12};
    CHECK(power.modifier == "POWER");
    CHECK(power.intervals == expected_power);

    const auto core = make_chord(0, 0, Face::B, 0, 0, Direction::Center);
    CHECK(core.name == "G");

    const auto minor_nine = make_chord(0, 0, Face::X, 0, 1, Direction::UpRight);
    const std::vector<int> expected_minor_nine{0, 3, 7, 10, 14};
    CHECK(minor_nine.name == "Am9");
    CHECK(minor_nine.intervals == expected_minor_nine);

    for (int bank = 0; bank < 3; ++bank) {
        for (int palette = 0; palette < 3; ++palette) {
            for (int face = 0; face < 4; ++face) {
                for (int direction = 0; direction < 9; ++direction) {
                    const auto chord = make_chord(0, 0, static_cast<Face>(face), bank, palette,
                                                  static_cast<Direction>(direction));
                    CHECK(!chord.intervals.empty());
                    CHECK(std::is_sorted(chord.intervals.begin(), chord.intervals.end()));
                    CHECK(std::set<int>(chord.intervals.begin(), chord.intervals.end()).size() == chord.intervals.size());
                    CHECK(chord.intervals.front() == 0);
                    CHECK(chord.intervals.back() <= 18);
                }
            }
        }
    }

    const auto stable_a = voice_chord(core, {}, false, VoicingProfile::Keys);
    const auto stable_b = voice_chord(core, {72, 76, 79}, false, VoicingProfile::Keys);
    CHECK(stable_a == stable_b);

    auto core_up = core;
    core_up.base_octave += 1;
    const auto octave_up = voice_chord(core_up, stable_a, false, VoicingProfile::Keys);
    CHECK(octave_up.size() == stable_a.size());
    for (std::size_t i = 0; i < stable_a.size() && i < octave_up.size(); ++i) {
        CHECK(octave_up[i] == stable_a[i] + 12);
    }

    const auto led = voice_chord(core, {60, 64, 67}, true, VoicingProfile::Keys);
    CHECK(!led.empty());
    const auto led_up = voice_chord(core_up, led, true, VoicingProfile::Keys);
    const double centre = static_cast<double>(std::accumulate(led_up.begin(), led_up.end(), 0)) /
                          static_cast<double>(led_up.size());
    CHECK(centre > 66.0);

    const auto sub = voice_chord(make_chord(0, 0, Face::A, 0, 1, Direction::DownRight), {}, false,
                                 VoicingProfile::Bass);
    CHECK(sub.size() <= 3U);
    CHECK(sub.back() - sub.front() <= 12);

    const auto diminished = make_chord(0, 0, Face::X, 1, 0, Direction::Center);
    const auto diminished_bass = voice_chord(diminished, {}, false, VoicingProfile::Bass);
    CHECK(diminished_bass.size() == 3U);
    CHECK(std::find(diminished_bass.begin(), diminished_bass.end(), diminished_bass.front() + 6) != diminished_bass.end());

    FacePressState faces;
    faces.press(Face::A);
    faces.press(Face::B);
    faces.press(Face::X);
    CHECK(faces.active() == Face::X);
    faces.release(Face::X);
    CHECK(faces.active() == Face::B);
    faces.press(Face::A);
    CHECK(faces.active() == Face::A);
    faces.clear();
    CHECK(!faces.active().has_value());
    CHECK(faces.count() == 0U);

    CHECK(!trigger_hysteresis(false, 17000));
    CHECK(trigger_hysteresis(false, 19000));
    CHECK(trigger_hysteresis(true, 13000));
    CHECK(!trigger_hysteresis(true, 11000));

    const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::filesystem::path config_path = std::filesystem::temp_directory_path() /
        ("brkchrd-config-test-" + std::to_string(unique) + ".cfg");
    const std::string malformed_config =
        "key 4\n"
        "param0 0.5\n"
        "paramX 0.7\n"
        "broken\n"
        "fx1type 2 trailing\n";
    std::string write_error;
    CHECK(atomic_write_text(config_path, malformed_config, &write_error));
    const auto parsed = read_numeric_config(config_path);
    CHECK(parsed.entries.size() == 3U);
    CHECK(!parsed.warnings.empty());
    CHECK(parse_indexed_key("param0", "param", 7) == 0);
    CHECK(!parse_indexed_key("paramX", "param", 7).has_value());
    CHECK(!parse_indexed_key("param7", "param", 7).has_value());
    CHECK(atomic_write_text(config_path, "key 9\n", &write_error));
    CHECK(!std::filesystem::exists(config_path.string() + ".tmp"));
    const auto rewritten = read_numeric_config(config_path);
    CHECK(rewritten.entries.size() == 1U);
    CHECK(rewritten.entries.front().key == "key");
    CHECK(static_cast<int>(rewritten.entries.front().value) == 9);
    std::error_code remove_error;
    std::filesystem::remove(config_path, remove_error);

    SynthEngine synth(48000.0);
    CHECK(SynthEngine::preset_count() >= 16);
    CHECK(SynthEngine::effect_count() >= 8);
    const auto first = synth.preset_info();
    CHECK(synth.voicing_profile() == VoicingProfile::Keys);
    synth.set_preset(13);
    const auto bass = synth.preset_info();
    CHECK(first.name != bass.name);
    CHECK(synth.voicing_profile() == VoicingProfile::Bass);
    synth.set_parameter(SynthParameter::Tone, 0.9F);
    CHECK(std::abs(synth.parameter(SynthParameter::Tone) - 0.9F) < 0.001F);
    synth.set_effect(0, {EffectType::Crusher, 0.5F, 0.2F});
    CHECK(synth.effect(0).type == EffectType::Crusher);
    synth.play_chord({48, 55, 60});
    auto audio = synth.render_copy(48000);
    CHECK(std::all_of(audio.begin(), audio.end(), [](float sample) { return std::isfinite(sample); }));
    CHECK(absolute_peak(audio) > 0.001F);
    CHECK(absolute_peak(audio) <= 1.001F);
    synth.release_chord();

    synth.set_effect(0, {EffectType::Delay, 0.9F, 0.2F});
    synth.set_effect(1, {EffectType::Off, 0.0F, 0.5F});
    synth.play_chord({60});
    static_cast<void>(synth.render_copy(4096));
    synth.all_notes_off();
    synth.set_effect(0, {EffectType::Off, 0.0F, 0.5F});
    synth.set_effect(0, {EffectType::Delay, 0.9F, 0.2F});
    const auto cleared_delay = synth.render_copy(2048);
    CHECK(absolute_peak(cleared_delay) < 0.00001F);

    SynthEngine deep_echo(48000.0);
    deep_echo.set_preset(1);
    deep_echo.set_effect(0, {EffectType::Delay, 0.84F, 0.86F});
    deep_echo.set_effect(1, {EffectType::Reverb, 0.40F, 0.72F});
    deep_echo.play_chord({48, 55, 60, 64});
    for (int block = 0; block < 240; ++block) {
        const auto echo_audio = deep_echo.render_copy(512);
        CHECK(std::all_of(echo_audio.begin(), echo_audio.end(), [](float sample) { return std::isfinite(sample); }));
        CHECK(absolute_peak(echo_audio) <= 1.001F);
        if (block == 24) deep_echo.release_chord();
    }

    SynthEngine rapid(48000.0);
    rapid.set_preset(1);
    const std::array<std::vector<int>, 4> rapid_chords{{
        {48, 55, 60, 64}, {50, 57, 62, 65}, {52, 59, 64, 67}, {53, 60, 65, 69}
    }};
    for (int hit = 0; hit < 240; ++hit) {
        rapid.play_chord(rapid_chords[static_cast<std::size_t>(hit) % rapid_chords.size()]);
        const auto attack = rapid.render_copy(512);
        CHECK(std::all_of(attack.begin(), attack.end(), [](float sample) { return std::isfinite(sample); }));
        CHECK(absolute_peak(attack) <= 1.001F);
        rapid.release_chord();
        const auto gap = rapid.render_copy(128);
        CHECK(std::all_of(gap.begin(), gap.end(), [](float sample) { return std::isfinite(sample); }));
    }

    rapid.play_chord({48, 55, 60, 64});
    static_cast<void>(rapid.render_copy(1024));
    rapid.change_chord({48, 55, 60, 65});
    const auto legato = rapid.render_copy(2048);
    CHECK(std::all_of(legato.begin(), legato.end(), [](float sample) { return std::isfinite(sample); }));
    CHECK(absolute_peak(legato) <= 1.001F);

    if (failures != 0) {
        std::cerr << failures << " BRKCHRD checks failed\n";
        return 1;
    }
    std::cout << "BRKCHRD checks passed\n";
    return 0;
}
