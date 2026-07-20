#pragma once

#include "brkchrd/music.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace brkchrd {

enum class SynthParameter { Tone = 0, Body, Motion, Attack, Release, Spread, Master, Count };
enum class EffectType { Off = 0, Chorus, Phaser, Tremolo, Drive, Crusher, Delay, Reverb, Count };

struct PresetInfo {
    std::string name;
    std::string character;
    std::string engine;
};

struct EffectSettings {
    EffectType type = EffectType::Off;
    float amount = 0.0F;
    float colour = 0.5F;
};

class SynthEngine {
public:
    explicit SynthEngine(double sample_rate = 48000.0);
    ~SynthEngine();
    SynthEngine(const SynthEngine&) = delete;
    SynthEngine& operator=(const SynthEngine&) = delete;

    void set_preset(int index);
    int preset_index() const;
    PresetInfo preset_info() const;
    VoicingProfile voicing_profile() const;
    static int preset_count();

    void set_parameter(SynthParameter parameter, float value);
    float parameter(SynthParameter parameter) const;
    void set_effect(int slot, EffectSettings settings);
    EffectSettings effect(int slot) const;
    static int effect_count();
    static std::string effect_name(EffectType type);

    void set_mode(PlayMode mode);
    void set_bpm(int bpm);
    void set_latch(bool latch);
    void play_chord(const std::vector<int>& notes);
    void release_chord();
    void all_notes_off();

    float output_peak() const;
    void render(float* interleaved_stereo, std::size_t frames);
    std::vector<float> render_copy(std::size_t frames);

private:
    struct Impl;
    Impl* impl_;
};

bool write_wav(const std::string& path, const std::vector<float>& stereo, int sample_rate);

} // namespace brkchrd
