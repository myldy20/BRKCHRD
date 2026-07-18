#pragma once

#include "brkchrd/music.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace brkchrd {

struct PresetInfo {
    std::string name;
    std::string character;
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
    static int preset_count();

    void set_mode(PlayMode mode);
    void set_bpm(int bpm);
    void set_latch(bool latch);
    void play_chord(const std::vector<int>& notes);
    void release_chord();
    void all_notes_off();

    void render(float* interleaved_stereo, std::size_t frames);
    std::vector<float> render_copy(std::size_t frames);

private:
    struct Impl;
    Impl* impl_;
};

bool write_wav(const std::string& path, const std::vector<float>& stereo, int sample_rate);

} // namespace brkchrd
