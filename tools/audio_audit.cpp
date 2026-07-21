#include "brkchrd/synth.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

using namespace brkchrd;

namespace {

constexpr int kRate = 48000;
constexpr std::size_t kBlock = 512U;

struct Stats {
    bool finite = true;
    float peak = 0.0F;
    double mean = 0.0;
    double rms = 0.0;
    float max_delta = 0.0F;
    double saturation = 0.0;
};

struct FxPair {
    const char* name;
    EffectSettings first;
    EffectSettings second;
};

EffectSettings fx(EffectType type, float amount, float colour) {
    return {type, amount, colour};
}

Stats analyse(const std::vector<float>& audio) {
    Stats result;
    if (audio.empty()) return result;
    double sum = 0.0;
    double squares = 0.0;
    std::size_t saturated = 0U;
    float previous_left = 0.0F;
    float previous_right = 0.0F;
    bool have_previous = false;
    for (std::size_t i = 0; i + 1U < audio.size(); i += 2U) {
        const float left = audio[i];
        const float right = audio[i + 1U];
        if (!std::isfinite(left) || !std::isfinite(right)) result.finite = false;
        result.peak = std::max(result.peak, std::max(std::abs(left), std::abs(right)));
        sum += static_cast<double>(left) + static_cast<double>(right);
        squares += static_cast<double>(left) * left + static_cast<double>(right) * right;
        if (std::abs(left) >= 0.96F) ++saturated;
        if (std::abs(right) >= 0.96F) ++saturated;
        if (have_previous) {
            result.max_delta = std::max(result.max_delta,
                std::max(std::abs(left - previous_left), std::abs(right - previous_right)));
        }
        previous_left = left;
        previous_right = right;
        have_previous = true;
    }
    const double samples = static_cast<double>(audio.size());
    result.mean = sum / samples;
    result.rms = std::sqrt(squares / samples);
    result.saturation = static_cast<double>(saturated) / samples;
    return result;
}

std::vector<float> concatenate(std::vector<float> first, const std::vector<float>& second) {
    first.insert(first.end(), second.begin(), second.end());
    return first;
}

const char* mode_name(PlayMode mode) {
    switch (mode) {
    case PlayMode::Pad: return "PAD";
    case PlayMode::Strum: return "STRUM";
    case PlayMode::Arp: return "ARP";
    case PlayMode::Pulse: return "PULSE";
    }
    return "UNKNOWN";
}

std::array<FxPair, 16> performance_pairs() {
    return {{
        {"A_REVERSE", fx(EffectType::Delay, 0.82F, 0.12F), fx(EffectType::Crusher, 0.42F, 0.20F)},
        {"A_STUTTER", fx(EffectType::Tremolo, 1.00F, 0.92F), fx(EffectType::Off, 0.0F, 0.5F)},
        {"A_CHOP", fx(EffectType::Tremolo, 1.00F, 1.00F), fx(EffectType::Crusher, 0.45F, 0.18F)},
        {"A_CRUSH", fx(EffectType::Crusher, 0.95F, 0.15F), fx(EffectType::Drive, 0.72F, 0.35F)},
        {"A_DRIVE", fx(EffectType::Drive, 0.98F, 0.24F), fx(EffectType::Phaser, 0.58F, 0.78F)},
        {"A_WASH", fx(EffectType::Reverb, 1.00F, 0.92F), fx(EffectType::Chorus, 0.72F, 0.82F)},
        {"A_DEEP_ECHO", fx(EffectType::Delay, 0.72F, 0.78F), fx(EffectType::Reverb, 0.32F, 0.66F)},
        {"A_PHASE", fx(EffectType::Phaser, 1.00F, 0.88F), fx(EffectType::Chorus, 0.82F, 0.76F)},
        {"B_ABYSS", fx(EffectType::Reverb, 1.00F, 1.00F), fx(EffectType::Delay, 1.00F, 0.98F)},
        {"B_RATETRAP", fx(EffectType::Tremolo, 1.00F, 0.99F), fx(EffectType::Delay, 0.56F, 0.10F)},
        {"B_GLITCH", fx(EffectType::Crusher, 1.00F, 0.06F), fx(EffectType::Tremolo, 0.86F, 0.95F)},
        {"B_BROKEN", fx(EffectType::Crusher, 0.88F, 0.04F), fx(EffectType::Phaser, 1.00F, 0.96F)},
        {"B_DOOM", fx(EffectType::Drive, 1.00F, 0.08F), fx(EffectType::Reverb, 0.82F, 0.32F)},
        {"B_SHIMMER", fx(EffectType::Chorus, 1.00F, 1.00F), fx(EffectType::Reverb, 1.00F, 0.96F)},
        {"B_TUNNEL", fx(EffectType::Delay, 1.00F, 1.00F), fx(EffectType::Phaser, 0.90F, 0.14F)},
        {"B_ALIEN", fx(EffectType::Phaser, 1.00F, 0.05F), fx(EffectType::Crusher, 0.62F, 0.08F)}
    }};
}

class Report {
public:
    explicit Report(const std::string& path) : output_(path) {
        output_ << "scenario,preset,mode,fx1,fx2,peak,rms,mean,max_delta,saturation,transition_jump,tail_ratio,render_ms,status\n";
    }

    void row(const std::string& scenario, int preset, PlayMode mode,
             const EffectSettings& first, const EffectSettings& second,
             const Stats& stats, double transition_jump, double tail_ratio,
             double render_ms, const std::string& status) {
        output_ << scenario << ',' << preset << ',' << mode_name(mode) << ','
                << SynthEngine::effect_name(first.type) << ',' << SynthEngine::effect_name(second.type) << ','
                << std::fixed << std::setprecision(7)
                << stats.peak << ',' << stats.rms << ',' << stats.mean << ',' << stats.max_delta << ','
                << stats.saturation << ',' << transition_jump << ',' << tail_ratio << ',' << render_ms << ','
                << status << '\n';
        ++rows_;
        if (status != "OK") {
            ++warnings_;
            std::cout << status << " " << scenario << " preset=" << preset
                      << " mode=" << mode_name(mode) << " peak=" << stats.peak
                      << " rms=" << stats.rms << " mean=" << stats.mean
                      << " delta=" << stats.max_delta << " sat=" << stats.saturation
                      << " jump=" << transition_jump << " tail=" << tail_ratio << '\n';
        }
    }

    int rows() const { return rows_; }
    int warnings() const { return warnings_; }

private:
    std::ofstream output_;
    int rows_ = 0;
    int warnings_ = 0;
};

std::string classify(const Stats& stats, double transition_jump = 0.0, double tail_ratio = 0.0) {
    if (!stats.finite) return "FAIL_NONFINITE";
    if (stats.peak > 1.001F) return "FAIL_OUTPUT_RANGE";
    if (std::abs(stats.mean) > 0.12) return "FAIL_DC";
    if (stats.max_delta > 1.50F) return "WARN_DELTA";
    if (transition_jump > 0.80) return "WARN_TRANSITION";
    if (stats.saturation > 0.25) return "WARN_SATURATION";
    if (tail_ratio > 1.15) return "WARN_TAIL_GROWTH";
    return "OK";
}

std::vector<float> render_timed(SynthEngine& synth, std::size_t frames, double& elapsed_ms) {
    const auto begin = std::chrono::steady_clock::now();
    auto audio = synth.render_copy(frames);
    const auto end = std::chrono::steady_clock::now();
    elapsed_ms = std::chrono::duration<double, std::milli>(end - begin).count();
    return audio;
}

void configure_extreme(SynthEngine& synth, bool maximum) {
    const float value = maximum ? 1.0F : 0.0F;
    synth.set_parameter(SynthParameter::Tone, value);
    synth.set_parameter(SynthParameter::Body, value);
    synth.set_parameter(SynthParameter::Motion, value);
    synth.set_parameter(SynthParameter::Attack, maximum ? 0.0F : 1.0F);
    synth.set_parameter(SynthParameter::Release, maximum ? 1.0F : 0.0F);
    synth.set_parameter(SynthParameter::Spread, value);
    synth.set_parameter(SynthParameter::Master, maximum ? 1.0F : 0.5F);
}

} // namespace

int main(int argc, char** argv) {
    const std::string path = argc > 1 ? argv[1] : "audio-audit.csv";
    const bool strict = argc > 2 && std::string(argv[2]) == "--strict";
    Report report(path);
    const std::array<PlayMode, 4> modes{PlayMode::Pad, PlayMode::Strum, PlayMode::Arp, PlayMode::Pulse};
    const std::vector<int> chord{48, 55, 60, 64, 67};
    const EffectSettings off{EffectType::Off, 0.0F, 0.5F};

    for (int preset = 0; preset < SynthEngine::preset_count(); ++preset) {
        for (PlayMode mode : modes) {
            SynthEngine synth(kRate);
            synth.set_preset(preset);
            synth.set_mode(mode);
            synth.set_effect(0, off);
            synth.set_effect(1, off);
            synth.play_chord(chord);
            double elapsed = 0.0;
            auto audio = render_timed(synth, static_cast<std::size_t>(kRate), elapsed);
            synth.release_chord();
            audio = concatenate(std::move(audio), synth.render_copy(kRate / 2U));
            const Stats stats = analyse(audio);
            report.row("BASELINE", preset, mode, off, off, stats, 0.0, 0.0, elapsed,
                       classify(stats));
        }
    }

    for (int preset = 0; preset < SynthEngine::preset_count(); ++preset) {
        for (bool maximum : {false, true}) {
            SynthEngine synth(kRate);
            synth.set_preset(preset);
            synth.set_mode(PlayMode::Pad);
            synth.set_effect(0, off);
            synth.set_effect(1, off);
            configure_extreme(synth, maximum);
            synth.play_chord(chord);
            double elapsed = 0.0;
            auto audio = render_timed(synth, static_cast<std::size_t>(kRate), elapsed);
            const Stats stats = analyse(audio);
            report.row(maximum ? "PARAM_MAX" : "PARAM_MIN", preset, PlayMode::Pad,
                       off, off, stats, 0.0, 0.0, elapsed, classify(stats));
        }
    }

    const std::array<float, 3> values{0.25F, 0.70F, 1.00F};
    for (int type = 1; type < SynthEngine::effect_count(); ++type) {
        for (float value : values) {
            for (int slot = 0; slot < 2; ++slot) {
                SynthEngine synth(kRate);
                synth.set_preset(1);
                synth.set_mode(PlayMode::Pad);
                const EffectSettings tested{static_cast<EffectType>(type), value, value};
                synth.set_effect(0, slot == 0 ? tested : off);
                synth.set_effect(1, slot == 1 ? tested : off);
                synth.play_chord(chord);
                double elapsed = 0.0;
                const auto audio = render_timed(synth, static_cast<std::size_t>(kRate), elapsed);
                const Stats stats = analyse(audio);
                report.row(slot == 0 ? "SINGLE_FX1" : "SINGLE_FX2", 1, PlayMode::Pad,
                           synth.effect(0), synth.effect(1), stats, 0.0, 0.0, elapsed,
                           classify(stats));
            }
        }
    }

    for (int first = 0; first < SynthEngine::effect_count(); ++first) {
        for (int second = 0; second < SynthEngine::effect_count(); ++second) {
            SynthEngine synth(kRate);
            synth.set_preset(1);
            synth.set_mode(PlayMode::Pad);
            const EffectSettings fx1{static_cast<EffectType>(first), first == 0 ? 0.0F : 1.0F, 1.0F};
            const EffectSettings fx2{static_cast<EffectType>(second), second == 0 ? 0.0F : 1.0F, 1.0F};
            synth.set_effect(0, fx1);
            synth.set_effect(1, fx2);
            synth.play_chord(chord);
            double elapsed = 0.0;
            const auto audio = render_timed(synth, static_cast<std::size_t>(kRate), elapsed);
            const Stats stats = analyse(audio);
            report.row("ALL_FX_PAIRS", 1, PlayMode::Pad, fx1, fx2, stats, 0.0, 0.0,
                       elapsed, classify(stats));
        }
    }

    for (const FxPair& pair : performance_pairs()) {
        for (int preset = 0; preset < SynthEngine::preset_count(); ++preset) {
            for (PlayMode mode : modes) {
                SynthEngine synth(kRate);
                synth.set_preset(preset);
                synth.set_mode(mode);
                synth.set_effect(0, off);
                synth.set_effect(1, off);
                synth.play_chord(chord);
                const auto before = synth.render_copy(4096U);
                const float previous_left = before[before.size() - 2U];
                const float previous_right = before[before.size() - 1U];
                synth.set_effect(0, pair.first);
                synth.set_effect(1, pair.second);
                double elapsed = 0.0;
                const auto active = render_timed(synth, static_cast<std::size_t>(kRate), elapsed);
                const double jump = std::max(std::abs(static_cast<double>(active[0] - previous_left)),
                                             std::abs(static_cast<double>(active[1] - previous_right)));
                synth.release_chord();
                const auto tail_start_audio = synth.render_copy(kRate);
                const Stats tail_start = analyse(tail_start_audio);
                const auto tail_end_audio = synth.render_copy(kRate * 5U);
                const std::size_t window = std::min<std::size_t>(tail_end_audio.size(), kRate * 2U);
                const std::vector<float> last(tail_end_audio.end() - static_cast<std::ptrdiff_t>(window), tail_end_audio.end());
                const Stats tail_end = analyse(last);
                const double tail_ratio = tail_start.rms > 0.000001 ? tail_end.rms / tail_start.rms : 0.0;
                const Stats stats = analyse(active);
                report.row(pair.name, preset, mode, pair.first, pair.second, stats, jump,
                           tail_ratio, elapsed, classify(stats, jump, tail_ratio));
            }
        }
    }

    for (int preset = 0; preset < SynthEngine::preset_count(); ++preset) {
        SynthEngine synth(kRate);
        synth.set_preset(preset);
        synth.set_mode(PlayMode::Pad);
        std::vector<float> combined;
        combined.reserve(180U * (kBlock + 96U) * 2U);
        for (int hit = 0; hit < 180; ++hit) {
            const int offset = (hit % 4) * 2;
            synth.play_chord({48 + offset, 55 + offset, 60 + offset, 64 + offset, 67 + offset});
            auto attack = synth.render_copy(kBlock);
            combined.insert(combined.end(), attack.begin(), attack.end());
            synth.release_chord();
            auto gap = synth.render_copy(96U);
            combined.insert(combined.end(), gap.begin(), gap.end());
        }
        const Stats stats = analyse(combined);
        report.row("RAPID_DEFAULT_FX", preset, PlayMode::Pad, synth.effect(0), synth.effect(1),
                   stats, 0.0, 0.0, 0.0, classify(stats));
    }

    {
        SynthEngine synth(kRate);
        synth.set_preset(1);
        synth.set_mode(PlayMode::Pad);
        configure_extreme(synth, true);
        synth.set_effect(0, {EffectType::Reverb, 1.0F, 1.0F});
        synth.set_effect(1, {EffectType::Delay, 1.0F, 1.0F});
        std::vector<int> dense;
        for (int note = 36; note < 60; ++note) dense.push_back(note);
        synth.play_chord(dense);
        std::vector<float> combined;
        double total_ms = 0.0;
        double max_block_ms = 0.0;
        for (int block = 0; block < 400; ++block) {
            double block_ms = 0.0;
            auto audio = render_timed(synth, kBlock, block_ms);
            total_ms += block_ms;
            max_block_ms = std::max(max_block_ms, block_ms);
            combined.insert(combined.end(), audio.begin(), audio.end());
        }
        const Stats stats = analyse(combined);
        const double average_ms = total_ms / 400.0;
        std::cout << "BENCHMARK average_block_ms=" << average_ms
                  << " max_block_ms=" << max_block_ms << " deadline_ms=10.6667\n";
        report.row("MAX_24_VOICE_BENCH", 1, PlayMode::Pad, synth.effect(0), synth.effect(1),
                   stats, max_block_ms, 0.0, average_ms, classify(stats));
    }

    std::cout << "audio audit rows=" << report.rows() << " warnings=" << report.warnings()
              << " report=" << path << '\n';
    if (strict && report.warnings() != 0) return 1;
    return 0;
}
