#include "brkchrd/synth.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace brkchrd;

namespace {

constexpr int kRate = 48000;
constexpr std::size_t kBlock = 512U;
const std::vector<int> kChord{48, 55, 60, 64, 67};
const EffectSettings kOff{EffectType::Off, 0.0F, 0.5F};

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

struct Rendered {
    std::vector<float> audio;
    double milliseconds = 0.0;
};

EffectSettings fx(EffectType type, float amount, float colour) {
    return {type, amount, colour};
}

const char* audit_mode_name(PlayMode mode) {
    switch (mode) {
    case PlayMode::Pad: return "PAD";
    case PlayMode::Strum: return "STRUM";
    case PlayMode::Arp: return "ARP";
    case PlayMode::Pulse: return "PULSE";
    }
    return "UNKNOWN";
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
    for (std::size_t index = 0; index + 1U < audio.size(); index += 2U) {
        const float left = audio[index];
        const float right = audio[index + 1U];
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
    const double count = static_cast<double>(audio.size());
    result.mean = sum / count;
    result.rms = std::sqrt(squares / count);
    result.saturation = static_cast<double>(saturated) / count;
    return result;
}

Rendered render_timed(SynthEngine& synth, std::size_t frames) {
    const auto begin = std::chrono::steady_clock::now();
    auto audio = synth.render_copy(frames);
    const auto end = std::chrono::steady_clock::now();
    return {std::move(audio), std::chrono::duration<double, std::milli>(end - begin).count()};
}

std::string classify(const Stats& stats, double transition_jump, double tail_ratio) {
    if (!stats.finite) return "FAIL_NONFINITE";
    if (stats.peak > 1.001F) return "FAIL_OUTPUT_RANGE";
    if (std::abs(stats.mean) > 0.12) return "FAIL_DC";
    if (stats.saturation > 0.25) return "WARN_SATURATION";
    if (stats.max_delta > 1.50F) return "WARN_DELTA";
    if (transition_jump > 0.80) return "WARN_TRANSITION";
    if (tail_ratio > 1.15) return "WARN_TAIL_GROWTH";
    return "OK";
}

class Report {
public:
    explicit Report(const std::string& path) : output_(path) {
        output_ << "scenario,preset,mode,fx1,fx2,peak,rms,mean,max_delta,saturation,transition_jump,tail_ratio,render_ms,status\n";
    }

    void add(const std::string& scenario, int preset, PlayMode mode,
             const EffectSettings& first, const EffectSettings& second,
             const Stats& stats, double transition_jump = 0.0,
             double tail_ratio = 0.0, double render_ms = 0.0) {
        const std::string status = classify(stats, transition_jump, tail_ratio);
        output_ << scenario << ',' << preset << ',' << audit_mode_name(mode) << ','
                << SynthEngine::effect_name(first.type) << ',' << SynthEngine::effect_name(second.type) << ','
                << std::fixed << std::setprecision(7)
                << stats.peak << ',' << stats.rms << ',' << stats.mean << ',' << stats.max_delta << ','
                << stats.saturation << ',' << transition_jump << ',' << tail_ratio << ',' << render_ms << ','
                << status << '\n';
        ++rows_;
        if (status != "OK") {
            ++warnings_;
            if (status.rfind("FAIL_", 0U) == 0U) ++failures_;
            std::cout << status << " " << scenario << " preset=" << preset
                      << " mode=" << audit_mode_name(mode)
                      << " fx=" << SynthEngine::effect_name(first.type) << '+'
                      << SynthEngine::effect_name(second.type)
                      << " peak=" << stats.peak << " rms=" << stats.rms
                      << " mean=" << stats.mean << " delta=" << stats.max_delta
                      << " sat=" << stats.saturation << " jump=" << transition_jump
                      << " tail=" << tail_ratio << '\n';
        }
    }

    int rows() const { return rows_; }
    int warnings() const { return warnings_; }
    int failures() const { return failures_; }

private:
    std::ofstream output_;
    int rows_ = 0;
    int warnings_ = 0;
    int failures_ = 0;
};

std::array<FxPair, 16> performance_pairs() {
    return {{
        {"A_REVERSE", fx(EffectType::Delay, 0.82F, 0.12F), fx(EffectType::Crusher, 0.42F, 0.20F)},
        {"A_STUTTER", fx(EffectType::Tremolo, 1.00F, 0.92F), kOff},
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

Stats run_sustained(int preset, PlayMode mode, EffectSettings first, EffectSettings second,
                    std::size_t frames, double& elapsed) {
    SynthEngine synth(kRate);
    synth.set_preset(preset);
    synth.set_mode(mode);
    synth.set_effect(0, first);
    synth.set_effect(1, second);
    synth.play_chord(kChord);
    Rendered rendered = render_timed(synth, frames);
    elapsed = rendered.milliseconds;
    return analyse(rendered.audio);
}

} // namespace

int main(int argc, char** argv) {
    const std::string path = argc > 1 ? argv[1] : "audio-audit.csv";
    const bool strict = argc > 2 && std::string(argv[2]) == "--strict";
    Report report(path);
    const std::array<PlayMode, 4> modes{PlayMode::Pad, PlayMode::Strum, PlayMode::Arp, PlayMode::Pulse};

    for (int preset = 0; preset < SynthEngine::preset_count(); ++preset) {
        for (PlayMode mode : modes) {
            double elapsed = 0.0;
            const Stats stats = run_sustained(preset, mode, kOff, kOff, kRate, elapsed);
            report.add("BASELINE", preset, mode, kOff, kOff, stats, 0.0, 0.0, elapsed);
        }
    }

    for (int preset = 0; preset < SynthEngine::preset_count(); ++preset) {
        for (bool maximum : {false, true}) {
            SynthEngine synth(kRate);
            synth.set_preset(preset);
            synth.set_mode(PlayMode::Pad);
            synth.set_effect(0, kOff);
            synth.set_effect(1, kOff);
            configure_extreme(synth, maximum);
            synth.play_chord(kChord);
            Rendered rendered = render_timed(synth, kRate);
            report.add(maximum ? "PARAM_MAX" : "PARAM_MIN", preset, PlayMode::Pad,
                       kOff, kOff, analyse(rendered.audio), 0.0, 0.0, rendered.milliseconds);
        }
    }

    const std::array<float, 3> values{0.25F, 0.70F, 1.00F};
    for (int type = 1; type < SynthEngine::effect_count(); ++type) {
        for (float value : values) {
            for (int slot = 0; slot < 2; ++slot) {
                const EffectSettings tested{static_cast<EffectType>(type), value, value};
                const EffectSettings first = slot == 0 ? tested : kOff;
                const EffectSettings second = slot == 1 ? tested : kOff;
                double elapsed = 0.0;
                const Stats stats = run_sustained(1, PlayMode::Pad, first, second, kRate, elapsed);
                report.add(slot == 0 ? "SINGLE_FX1" : "SINGLE_FX2", 1, PlayMode::Pad,
                           first, second, stats, 0.0, 0.0, elapsed);
            }
        }
    }

    for (int preset = 0; preset < SynthEngine::preset_count(); ++preset) {
        for (int first_type = 0; first_type < SynthEngine::effect_count(); ++first_type) {
            for (int second_type = 0; second_type < SynthEngine::effect_count(); ++second_type) {
                const EffectSettings first{static_cast<EffectType>(first_type), first_type == 0 ? 0.0F : 1.0F, 1.0F};
                const EffectSettings second{static_cast<EffectType>(second_type), second_type == 0 ? 0.0F : 1.0F, 1.0F};
                double elapsed = 0.0;
                const Stats stats = run_sustained(preset, PlayMode::Pad, first, second, kRate / 2U, elapsed);
                report.add("ALL_FX_PAIRS", preset, PlayMode::Pad, first, second, stats, 0.0, 0.0, elapsed);
            }
        }
    }

    const auto pairs = performance_pairs();
    for (const FxPair& pair : pairs) {
        for (int preset = 0; preset < SynthEngine::preset_count(); ++preset) {
            for (PlayMode mode : modes) {
                SynthEngine synth(kRate);
                synth.set_preset(preset);
                synth.set_mode(mode);
                synth.set_effect(0, kOff);
                synth.set_effect(1, kOff);
                synth.play_chord(kChord);
                const auto before = synth.render_copy(4096U);
                const float previous_left = before[before.size() - 2U];
                const float previous_right = before[before.size() - 1U];
                synth.set_effect(0, pair.first);
                synth.set_effect(1, pair.second);
                Rendered active = render_timed(synth, kRate / 2U);
                const double jump = std::max(std::abs(static_cast<double>(active.audio[0] - previous_left)),
                                             std::abs(static_cast<double>(active.audio[1] - previous_right)));
                report.add(pair.name, preset, mode, pair.first, pair.second,
                           analyse(active.audio), jump, 0.0, active.milliseconds);
            }
        }
    }

    for (const FxPair& pair : pairs) {
        SynthEngine synth(kRate);
        synth.set_preset(1);
        synth.set_mode(PlayMode::Pad);
        synth.set_effect(0, pair.first);
        synth.set_effect(1, pair.second);
        synth.play_chord(kChord);
        static_cast<void>(synth.render_copy(kRate));
        synth.release_chord();
        const Stats tail_start = analyse(synth.render_copy(kRate));
        const auto long_tail = synth.render_copy(kRate * 5U);
        const std::size_t window = static_cast<std::size_t>(kRate) * 2U;
        const std::vector<float> tail_end_audio(long_tail.end() - static_cast<std::ptrdiff_t>(window), long_tail.end());
        const Stats tail_end = analyse(tail_end_audio);
        const double ratio = tail_start.rms > 0.000001 ? tail_end.rms / tail_start.rms : 0.0;
        report.add(std::string(pair.name) + "_TAIL", 1, PlayMode::Pad,
                   pair.first, pair.second, tail_end, 0.0, ratio, 0.0);
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
            const auto attack = synth.render_copy(kBlock);
            combined.insert(combined.end(), attack.begin(), attack.end());
            synth.release_chord();
            const auto gap = synth.render_copy(96U);
            combined.insert(combined.end(), gap.begin(), gap.end());
        }
        report.add("RAPID_DEFAULT_FX", preset, PlayMode::Pad, synth.effect(0), synth.effect(1),
                   analyse(combined));
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
        double maximum_ms = 0.0;
        for (int block = 0; block < 400; ++block) {
            Rendered rendered = render_timed(synth, kBlock);
            total_ms += rendered.milliseconds;
            maximum_ms = std::max(maximum_ms, rendered.milliseconds);
            combined.insert(combined.end(), rendered.audio.begin(), rendered.audio.end());
        }
        const double average_ms = total_ms / 400.0;
        std::cout << "BENCHMARK average_block_ms=" << average_ms
                  << " max_block_ms=" << maximum_ms << " deadline_ms=10.6667\n";
        report.add("MAX_24_VOICE_BENCH", 1, PlayMode::Pad, synth.effect(0), synth.effect(1),
                   analyse(combined), 0.0, 0.0, average_ms);
    }

    std::cout << "audio audit rows=" << report.rows() << " warnings=" << report.warnings()
              << " failures=" << report.failures() << " report=" << path << '\n';
    if (strict && report.failures() != 0) return 1;
    return 0;
}
