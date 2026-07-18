#include "brkchrd/synth.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <memory>

namespace brkchrd {
namespace {

constexpr double kPi = 3.14159265358979323846;

enum class Wave { Sine, Triangle, Saw, Square };

struct Preset {
    const char* name;
    const char* character;
    Wave wave1;
    Wave wave2;
    float mix2;
    float detune_cents;
    float attack;
    float release;
    float cutoff;
    float chorus;
    float delay;
    float feedback;
    float reverb;
    float drive;
};

constexpr std::array<Preset, 8> kPresets{{
    {"WARM PAD", "slow analog air", Wave::Saw, Wave::Triangle, 0.42f, 7.0f, 0.18f, 1.35f, 0.22f, 0.48f, 0.22f, 0.32f, 0.32f, 1.25f},
    {"GLASS KEYS", "clear FM-like bells", Wave::Sine, Wave::Triangle, 0.62f, 1200.0f, 0.006f, 0.85f, 0.68f, 0.22f, 0.28f, 0.38f, 0.38f, 1.05f},
    {"REED ROOM", "soft electric reed", Wave::Square, Wave::Sine, 0.38f, 12.0f, 0.025f, 0.72f, 0.38f, 0.34f, 0.18f, 0.28f, 0.24f, 1.45f},
    {"HOLLOW TAPE", "worn square chorus", Wave::Square, Wave::Triangle, 0.45f, -9.0f, 0.045f, 1.5f, 0.17f, 0.62f, 0.35f, 0.42f, 0.48f, 1.55f},
    {"STRING CLOUD", "wide ensemble swell", Wave::Saw, Wave::Saw, 0.55f, 11.0f, 0.28f, 2.2f, 0.31f, 0.72f, 0.27f, 0.36f, 0.58f, 1.18f},
    {"DARK ORGAN", "dense low harmonics", Wave::Square, Wave::Sine, 0.32f, 1900.0f, 0.01f, 0.48f, 0.13f, 0.18f, 0.12f, 0.25f, 0.22f, 1.8f},
    {"TAPE CHOIR", "breathing unstable pad", Wave::Triangle, Wave::Saw, 0.36f, -14.0f, 0.42f, 2.6f, 0.2f, 0.68f, 0.31f, 0.46f, 0.64f, 1.38f},
    {"SUB DRONE", "slow black foundation", Wave::Sine, Wave::Saw, 0.18f, -1200.0f, 0.65f, 3.2f, 0.08f, 0.24f, 0.36f, 0.52f, 0.55f, 2.1f},
}};

float wave_sample(Wave wave, double phase) {
    phase -= std::floor(phase);
    switch (wave) {
        case Wave::Sine: return static_cast<float>(std::sin(2.0 * kPi * phase));
        case Wave::Triangle: return static_cast<float>(4.0 * std::abs(phase - 0.5) - 1.0);
        case Wave::Saw: return static_cast<float>(2.0 * phase - 1.0);
        case Wave::Square: return phase < 0.5 ? 1.0f : -1.0f;
    }
    return 0.0f;
}

float midi_frequency(int note) {
    return 440.0f * std::pow(2.0f, (static_cast<float>(note) - 69.0f) / 12.0f);
}

float cents_ratio(float cents) {
    return std::pow(2.0f, cents / 1200.0f);
}

struct Voice {
    bool active = false;
    bool gate = false;
    int note = 60;
    double phase1 = 0.0;
    double phase2 = 0.0;
    float env = 0.0f;
    float lp_l = 0.0f;
    float lp_r = 0.0f;
    float pan = 0.0f;
    std::uint64_t age = 0;
};

struct ScheduledNote {
    std::int64_t samples = 0;
    int note = 60;
};

float soft_clip(float x, float drive) {
    return std::tanh(x * drive) / std::tanh(std::max(1.0f, drive));
}

void write_u16(std::ofstream& out, std::uint16_t v) {
    out.put(static_cast<char>(v & 0xff));
    out.put(static_cast<char>((v >> 8) & 0xff));
}

void write_u32(std::ofstream& out, std::uint32_t v) {
    out.put(static_cast<char>(v & 0xff));
    out.put(static_cast<char>((v >> 8) & 0xff));
    out.put(static_cast<char>((v >> 16) & 0xff));
    out.put(static_cast<char>((v >> 24) & 0xff));
}

} // namespace

struct SynthEngine::Impl {
    explicit Impl(double sr)
        : sample_rate(sr),
          chorus_l(4096, 0.0f), chorus_r(4096, 0.0f),
          delay_l(static_cast<std::size_t>(sr * 2.5), 0.0f), delay_r(static_cast<std::size_t>(sr * 2.5), 0.0f),
          verb_l(static_cast<std::size_t>(sr * 0.71), 0.0f), verb_r(static_cast<std::size_t>(sr * 0.83), 0.0f) {}

    double sample_rate;
    mutable std::mutex mutex;
    int preset = 0;
    PlayMode mode = PlayMode::Pad;
    int bpm = 92;
    bool latch = false;
    std::vector<int> held_notes;
    std::array<Voice, 24> voices{};
    std::vector<ScheduledNote> scheduled;
    std::uint64_t voice_age = 0;
    std::int64_t rhythm_countdown = 0;
    std::size_t arp_index = 0;
    int arp_note = -1;
    double lfo_phase = 0.0;
    std::vector<float> chorus_l;
    std::vector<float> chorus_r;
    std::size_t chorus_pos = 0;
    std::vector<float> delay_l;
    std::vector<float> delay_r;
    std::size_t delay_pos = 0;
    std::vector<float> verb_l;
    std::vector<float> verb_r;
    std::size_t verb_pos_l = 0;
    std::size_t verb_pos_r = 0;

    const Preset& patch() const { return kPresets[static_cast<std::size_t>(preset)]; }

    void stop_note(int note) {
        for (auto& v : voices) {
            if (v.active && v.note == note) v.gate = false;
        }
    }

    void release_all() {
        for (auto& v : voices) v.gate = false;
    }

    void start_note(int note) {
        Voice* selected = nullptr;
        for (auto& v : voices) {
            if (!v.active) {
                selected = &v;
                break;
            }
        }
        if (selected == nullptr) {
            selected = &*std::min_element(voices.begin(), voices.end(), [](const Voice& a, const Voice& b) {
                if (a.env != b.env) return a.env < b.env;
                return a.age < b.age;
            });
        }
        selected->active = true;
        selected->gate = true;
        selected->note = note;
        selected->phase1 = 0.0;
        selected->phase2 = 0.13;
        selected->env = std::min(selected->env, 0.12f);
        selected->lp_l = selected->lp_r = 0.0f;
        selected->pan = std::clamp((static_cast<float>((note % 12) - 6) / 8.0f), -0.72f, 0.72f);
        selected->age = ++voice_age;
    }

    std::int64_t beat_samples(double beats) const {
        return static_cast<std::int64_t>(sample_rate * 60.0 / static_cast<double>(std::max(40, bpm)) * beats);
    }

    void schedule_chord(const std::vector<int>& notes, bool strum) {
        release_all();
        scheduled.clear();
        const std::int64_t spacing = strum ? static_cast<std::int64_t>(sample_rate * 0.028) : 0;
        for (std::size_t i = 0; i < notes.size(); ++i) {
            scheduled.push_back({spacing * static_cast<std::int64_t>(i), notes[i]});
        }
    }

    void set_notes(const std::vector<int>& notes) {
        held_notes = notes;
        arp_index = 0;
        rhythm_countdown = 0;
        if (mode == PlayMode::Pad) schedule_chord(notes, false);
        else if (mode == PlayMode::Strum) schedule_chord(notes, true);
        else release_all();
    }

    void process_scheduled() {
        for (auto it = scheduled.begin(); it != scheduled.end();) {
            if (it->samples <= 0) {
                start_note(it->note);
                it = scheduled.erase(it);
            } else {
                --it->samples;
                ++it;
            }
        }
    }

    void process_rhythm() {
        if (held_notes.empty() || (mode != PlayMode::Arp && mode != PlayMode::Pulse)) return;
        if (rhythm_countdown > 0) {
            --rhythm_countdown;
            return;
        }
        if (mode == PlayMode::Arp) {
            if (arp_note >= 0) stop_note(arp_note);
            arp_note = held_notes[arp_index % held_notes.size()];
            start_note(arp_note);
            arp_index = (arp_index + 1) % held_notes.size();
            rhythm_countdown = beat_samples(0.5);
        } else {
            release_all();
            for (int note : held_notes) start_note(note);
            rhythm_countdown = beat_samples(1.0);
        }
    }

    void render_locked(float* out, std::size_t frames) {
        const Preset p = patch();
        const float attack_step = 1.0f / static_cast<float>(std::max(1.0, p.attack * sample_rate));
        const float release_step = 1.0f / static_cast<float>(std::max(1.0, p.release * sample_rate));
        const float cutoff_hz = 70.0f + p.cutoff * p.cutoff * 15000.0f;
        const float lp_alpha = 1.0f - std::exp(-2.0f * static_cast<float>(kPi) * cutoff_hz / static_cast<float>(sample_rate));
        const float detune = cents_ratio(p.detune_cents);
        const std::size_t delay_len = std::max<std::size_t>(1, static_cast<std::size_t>(sample_rate * 60.0 / std::max(40, bpm) * 0.75));

        for (std::size_t frame = 0; frame < frames; ++frame) {
            process_scheduled();
            process_rhythm();
            float left = 0.0f;
            float right = 0.0f;
            int active_count = 0;
            const float drift = 1.0f + 0.0015f * std::sin(static_cast<float>(2.0 * kPi * lfo_phase));
            for (auto& v : voices) {
                if (!v.active) continue;
                if (v.gate) v.env = std::min(1.0f, v.env + attack_step);
                else v.env = std::max(0.0f, v.env - release_step);
                if (!v.gate && v.env <= 0.0001f) {
                    v.active = false;
                    continue;
                }
                ++active_count;
                const float f1 = midi_frequency(v.note) * drift;
                const float f2 = f1 * detune;
                const float s1 = wave_sample(p.wave1, v.phase1);
                const float s2 = wave_sample(p.wave2, v.phase2);
                v.phase1 += static_cast<double>(f1 / static_cast<float>(sample_rate));
                v.phase2 += static_cast<double>(f2 / static_cast<float>(sample_rate));
                v.phase1 -= std::floor(v.phase1);
                v.phase2 -= std::floor(v.phase2);
                float sample = (s1 * (1.0f - p.mix2) + s2 * p.mix2) * v.env * 0.19f;
                const float pan_l = std::sqrt(0.5f * (1.0f - v.pan));
                const float pan_r = std::sqrt(0.5f * (1.0f + v.pan));
                const float raw_l = sample * pan_l;
                const float raw_r = sample * pan_r;
                v.lp_l += lp_alpha * (raw_l - v.lp_l);
                v.lp_r += lp_alpha * (raw_r - v.lp_r);
                left += v.lp_l;
                right += v.lp_r;
            }
            if (active_count > 8) {
                const float norm = 8.0f / static_cast<float>(active_count);
                left *= norm;
                right *= norm;
            }

            const float chorus_phase = static_cast<float>(0.5 + 0.5 * std::sin(2.0 * kPi * lfo_phase));
            const std::size_t chorus_delay = 120 + static_cast<std::size_t>(chorus_phase * 420.0f);
            const std::size_t read_pos = (chorus_pos + chorus_l.size() - chorus_delay) % chorus_l.size();
            const float ch_l = chorus_l[read_pos];
            const float ch_r = chorus_r[(read_pos + 73) % chorus_r.size()];
            chorus_l[chorus_pos] = left;
            chorus_r[chorus_pos] = right;
            chorus_pos = (chorus_pos + 1) % chorus_l.size();
            left = left * (1.0f - p.chorus * 0.35f) + ch_r * p.chorus * 0.35f;
            right = right * (1.0f - p.chorus * 0.35f) + ch_l * p.chorus * 0.35f;

            const std::size_t d_read = (delay_pos + delay_l.size() - std::min(delay_len, delay_l.size() - 1)) % delay_l.size();
            const float dl = delay_l[d_read];
            const float dr = delay_r[d_read];
            delay_l[delay_pos] = left + dr * p.feedback;
            delay_r[delay_pos] = right + dl * p.feedback;
            delay_pos = (delay_pos + 1) % delay_l.size();
            left += dl * p.delay;
            right += dr * p.delay;

            const float vl = verb_l[verb_pos_l];
            const float vr = verb_r[verb_pos_r];
            verb_l[verb_pos_l] = left + vr * 0.67f;
            verb_r[verb_pos_r] = right + vl * 0.64f;
            verb_pos_l = (verb_pos_l + 1) % verb_l.size();
            verb_pos_r = (verb_pos_r + 1) % verb_r.size();
            left += (vl + vr * 0.35f) * p.reverb * 0.42f;
            right += (vr + vl * 0.35f) * p.reverb * 0.42f;

            lfo_phase += 0.19 / sample_rate;
            if (lfo_phase >= 1.0) lfo_phase -= 1.0;
            out[frame * 2] = soft_clip(left, p.drive);
            out[frame * 2 + 1] = soft_clip(right, p.drive);
        }
    }
};

SynthEngine::SynthEngine(double sample_rate) : impl_(new Impl(sample_rate)) {}
SynthEngine::~SynthEngine() { delete impl_; }

void SynthEngine::set_preset(int index) {
    std::scoped_lock lock(impl_->mutex);
    const int count = static_cast<int>(kPresets.size());
    index %= count;
    if (index < 0) index += count;
    impl_->preset = index;
}

int SynthEngine::preset_index() const {
    std::scoped_lock lock(impl_->mutex);
    return impl_->preset;
}

PresetInfo SynthEngine::preset_info() const {
    std::scoped_lock lock(impl_->mutex);
    const Preset& p = impl_->patch();
    return {p.name, p.character};
}

int SynthEngine::preset_count() { return static_cast<int>(kPresets.size()); }

void SynthEngine::set_mode(PlayMode mode) {
    std::scoped_lock lock(impl_->mutex);
    impl_->mode = mode;
    if (!impl_->held_notes.empty()) impl_->set_notes(impl_->held_notes);
}

void SynthEngine::set_bpm(int bpm) {
    std::scoped_lock lock(impl_->mutex);
    impl_->bpm = std::clamp(bpm, 40, 240);
}

void SynthEngine::set_latch(bool latch) {
    std::scoped_lock lock(impl_->mutex);
    impl_->latch = latch;
    if (!latch && impl_->held_notes.empty()) impl_->release_all();
}

void SynthEngine::play_chord(const std::vector<int>& notes) {
    std::scoped_lock lock(impl_->mutex);
    impl_->set_notes(notes);
}

void SynthEngine::release_chord() {
    std::scoped_lock lock(impl_->mutex);
    if (impl_->latch) return;
    impl_->held_notes.clear();
    impl_->release_all();
    impl_->scheduled.clear();
    impl_->arp_note = -1;
}

void SynthEngine::all_notes_off() {
    std::scoped_lock lock(impl_->mutex);
    impl_->held_notes.clear();
    impl_->scheduled.clear();
    for (auto& voice : impl_->voices) {
        voice.gate = false;
        voice.env = 0.0f;
        voice.active = false;
    }
}

void SynthEngine::render(float* interleaved_stereo, std::size_t frames) {
    std::scoped_lock lock(impl_->mutex);
    impl_->render_locked(interleaved_stereo, frames);
}

std::vector<float> SynthEngine::render_copy(std::size_t frames) {
    std::vector<float> result(frames * 2, 0.0f);
    render(result.data(), frames);
    return result;
}

bool write_wav(const std::string& path, const std::vector<float>& stereo, int sample_rate) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    const std::uint32_t data_bytes = static_cast<std::uint32_t>(stereo.size() * sizeof(std::int16_t));
    out.write("RIFF", 4);
    write_u32(out, 36u + data_bytes);
    out.write("WAVEfmt ", 8);
    write_u32(out, 16);
    write_u16(out, 1);
    write_u16(out, 2);
    write_u32(out, static_cast<std::uint32_t>(sample_rate));
    write_u32(out, static_cast<std::uint32_t>(sample_rate * 4));
    write_u16(out, 4);
    write_u16(out, 16);
    out.write("data", 4);
    write_u32(out, data_bytes);
    for (float sample : stereo) {
        const float clipped = std::clamp(sample, -1.0f, 1.0f);
        const auto pcm = static_cast<std::int16_t>(std::lrint(clipped * 32767.0f));
        write_u16(out, static_cast<std::uint16_t>(pcm));
    }
    return static_cast<bool>(out);
}

} // namespace brkchrd
