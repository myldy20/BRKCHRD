#include "brkchrd/synth.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <vector>

namespace brkchrd {
namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr float kEpsilon = 0.00001F;

enum class EngineKind { Analog, Electric, Organ, Strings, Choir, Pluck, Glass, Doom, Reed, Sub };

struct Patch {
    const char* name;
    const char* character;
    const char* engine_name;
    EngineKind engine;
    VoicingProfile voicing;
    std::array<float, static_cast<std::size_t>(SynthParameter::Count)> controls;
    EffectSettings fx1;
    EffectSettings fx2;
    float decay;
    float sustain;
    float level;
};

constexpr std::array<Patch, 16> kPatches{{
    {"AMBER KEYS", "warm rounded electric keys", "ELECTRIC TINES", EngineKind::Electric, VoicingProfile::Keys,
     {0.50F,0.66F,0.12F,0.02F,0.48F,0.48F,0.82F}, {EffectType::Tremolo,0.10F,0.30F}, {EffectType::Reverb,0.24F,0.56F}, 0.82F, 0.18F, 0.98F},
    {"ANALOG BLOOM", "soft wide analog chord pad", "DUAL ANALOG", EngineKind::Analog, VoicingProfile::Pad,
     {0.42F,0.62F,0.20F,0.24F,0.68F,0.72F,0.78F}, {EffectType::Chorus,0.34F,0.42F}, {EffectType::Reverb,0.28F,0.62F}, 0.72F, 0.84F, 0.92F},
    {"MOON ORGAN", "mellow sustained organ", "ADDITIVE ORGAN", EngineKind::Organ, VoicingProfile::Organ,
     {0.38F,0.72F,0.06F,0.02F,0.34F,0.34F,0.76F}, {EffectType::Chorus,0.18F,0.34F}, {EffectType::Reverb,0.32F,0.68F}, 0.24F, 0.98F, 0.88F},
    {"CINEMA STRINGS", "smooth restrained ensemble", "STRING ENSEMBLE", EngineKind::Strings, VoicingProfile::Pad,
     {0.36F,0.54F,0.26F,0.34F,0.72F,0.82F,0.72F}, {EffectType::Chorus,0.46F,0.54F}, {EffectType::Reverb,0.36F,0.72F}, 0.88F, 0.90F, 0.82F},
    {"VELVET CHOIR", "dark breathless vowel pad", "FORMANT CHOIR", EngineKind::Choir, VoicingProfile::Choir,
     {0.34F,0.68F,0.24F,0.30F,0.78F,0.70F,0.70F}, {EffectType::Chorus,0.32F,0.62F}, {EffectType::Reverb,0.34F,0.74F}, 0.94F, 0.88F, 0.78F},
    {"MUTED PLUCK", "soft woody articulated pluck", "RESONANT PLUCK", EngineKind::Pluck, VoicingProfile::Pluck,
     {0.48F,0.62F,0.12F,0.01F,0.24F,0.46F,0.82F}, {EffectType::Off,0.0F,0.5F}, {EffectType::Delay,0.18F,0.32F}, 0.48F, 0.00F, 0.94F},
    {"GLASS HARP", "gentle glass harmonics", "PHASE MOD", EngineKind::Glass, VoicingProfile::Pluck,
     {0.46F,0.42F,0.18F,0.01F,0.54F,0.62F,0.72F}, {EffectType::Chorus,0.16F,0.54F}, {EffectType::Reverb,0.36F,0.78F}, 0.72F, 0.06F, 0.74F},
    {"NOIR REED", "soft smoky reed ensemble", "PULSE REED", EngineKind::Reed, VoicingProfile::Keys,
     {0.34F,0.64F,0.18F,0.08F,0.52F,0.38F,0.76F}, {EffectType::Tremolo,0.10F,0.42F}, {EffectType::Reverb,0.18F,0.54F}, 0.58F, 0.80F, 0.78F},
    {"DUST PIANO", "worn soft electric piano", "ELECTRIC TINES", EngineKind::Electric, VoicingProfile::Keys,
     {0.62F,0.44F,0.18F,0.01F,0.38F,0.52F,0.80F}, {EffectType::Tremolo,0.14F,0.36F}, {EffectType::Delay,0.16F,0.28F}, 0.70F, 0.08F, 0.92F},
    {"STRING HAZE", "wide slow moving strings", "STRING ENSEMBLE", EngineKind::Strings, VoicingProfile::Pad,
     {0.44F,0.46F,0.40F,0.48F,0.78F,0.90F,0.68F}, {EffectType::Chorus,0.58F,0.64F}, {EffectType::Reverb,0.42F,0.80F}, 0.96F, 0.90F, 0.76F},
    {"CHAPEL AIR", "airy restrained church organ", "ADDITIVE ORGAN", EngineKind::Organ, VoicingProfile::Organ,
     {0.52F,0.58F,0.10F,0.06F,0.48F,0.46F,0.72F}, {EffectType::Phaser,0.10F,0.28F}, {EffectType::Reverb,0.44F,0.82F}, 0.34F, 0.96F, 0.76F},
    {"TAPE CHOIR", "worn moving vocal tape", "FORMANT CHOIR", EngineKind::Choir, VoicingProfile::Choir,
     {0.40F,0.72F,0.46F,0.42F,0.84F,0.76F,0.66F}, {EffectType::Chorus,0.44F,0.70F}, {EffectType::Delay,0.18F,0.58F}, 0.98F, 0.86F, 0.72F},
    {"DOOM CHORDS", "controlled saturated power stack", "POWER STACK", EngineKind::Doom, VoicingProfile::Heavy,
     {0.28F,0.72F,0.12F,0.03F,0.42F,0.36F,0.62F}, {EffectType::Drive,0.42F,0.34F}, {EffectType::Reverb,0.14F,0.32F}, 0.48F, 0.90F, 0.62F},
    {"SUB ALTAR", "root fifth and octave foundation", "SUB HARMONICS", EngineKind::Sub, VoicingProfile::Bass,
     {0.16F,0.92F,0.08F,0.18F,0.70F,0.22F,0.58F}, {EffectType::Drive,0.14F,0.18F}, {EffectType::Off,0.0F,0.5F}, 0.88F, 0.98F, 0.56F},
    {"FROZEN GLASS", "bright unstable metallic glass", "PHASE MOD", EngineKind::Glass, VoicingProfile::Pluck,
     {0.72F,0.30F,0.38F,0.01F,0.58F,0.74F,0.66F}, {EffectType::Phaser,0.24F,0.70F}, {EffectType::Reverb,0.44F,0.84F}, 0.66F, 0.06F, 0.64F},
    {"WIRE PLUCK", "dry metallic short pluck", "RESONANT PLUCK", EngineKind::Pluck, VoicingProfile::Pluck,
     {0.72F,0.46F,0.20F,0.01F,0.20F,0.58F,0.76F}, {EffectType::Drive,0.08F,0.24F}, {EffectType::Delay,0.24F,0.40F}, 0.34F, 0.00F, 0.72F},
}};

float clamp01(float value) { return std::clamp(value, 0.0F, 1.0F); }
float midi_frequency(int note) { return 440.0F * std::pow(2.0F, (static_cast<float>(note) - 69.0F) / 12.0F); }
float sine(double phase) { return static_cast<float>(std::sin(2.0 * kPi * phase)); }
float saw(double phase) { phase -= std::floor(phase); return static_cast<float>(phase * 2.0 - 1.0); }
float triangle(double phase) { phase -= std::floor(phase); return static_cast<float>(1.0 - 4.0 * std::abs(phase - 0.5)); }
float square(double phase, float width = 0.5F) { phase -= std::floor(phase); return phase < width ? 1.0F : -1.0F; }
float soft_clip(float value, float drive) {
    const float d = std::max(1.0F, drive);
    return std::clamp(std::tanh(value * d) / std::tanh(d), -1.0F, 1.0F);
}

struct Voice {
    bool active = false;
    bool gate = false;
    bool fast_release = false;
    int note = 60;
    double p1 = 0.0;
    double p2 = 0.0;
    double p3 = 0.0;
    double p4 = 0.0;
    float env = 0.0F;
    float life = 0.0F;
    float low = 0.0F;
    float band = 0.0F;
    float pan = 0.0F;
    std::uint32_t noise = 1U;
    std::uint64_t age = 0U;
};

struct ScheduledNote { std::int64_t samples = 0; int note = 60; };

struct EffectState {
    explicit EffectState(std::size_t size = 1U) : left(size, 0.0F), right(size, 0.0F) {}
    std::vector<float> left;
    std::vector<float> right;
    std::size_t position = 0U;
    std::size_t valid_samples = 0U;
    double lfo = 0.0;
    float ap_l1 = 0.0F;
    float ap_l2 = 0.0F;
    float ap_r1 = 0.0F;
    float ap_r2 = 0.0F;
    int hold = 0;
    float held_l = 0.0F;
    float held_r = 0.0F;

    bool readable(std::size_t index) const {
        const std::size_t distance = (position + left.size() - index) % left.size();
        return distance > 0U && distance <= valid_samples;
    }

    float read_left(std::size_t index) const { return readable(index) ? left[index] : 0.0F; }
    float read_right(std::size_t index) const { return readable(index) ? right[index] : 0.0F; }

    void reset() {
        position = 0U;
        valid_samples = 0U;
        lfo = 0.0;
        ap_l1 = ap_l2 = ap_r1 = ap_r2 = 0.0F;
        hold = 0;
        held_l = held_r = 0.0F;
    }
};

void write_u16(std::ofstream& out, std::uint16_t value) {
    out.put(static_cast<char>(value & 0xffU)); out.put(static_cast<char>((value >> 8U) & 0xffU));
}
void write_u32(std::ofstream& out, std::uint32_t value) {
    out.put(static_cast<char>(value & 0xffU)); out.put(static_cast<char>((value >> 8U) & 0xffU));
    out.put(static_cast<char>((value >> 16U) & 0xffU)); out.put(static_cast<char>((value >> 24U) & 0xffU));
}

} // namespace

struct SynthEngine::Impl {
    explicit Impl(double rate)
        : sample_rate(rate), fx_state{EffectState(static_cast<std::size_t>(rate * 2.2)), EffectState(static_cast<std::size_t>(rate * 2.2))} {
        load_patch(0);
    }

    double sample_rate;
    mutable std::mutex mutex;
    int preset = 0;
    std::array<float, static_cast<std::size_t>(SynthParameter::Count)> controls{};
    std::array<EffectSettings, 2> effects{};
    PlayMode mode = PlayMode::Pad;
    int bpm = 92;
    bool latch = false;
    std::vector<int> held_notes;
    std::array<Voice, 24> voices{};
    std::vector<ScheduledNote> scheduled;
    std::uint64_t voice_age = 0U;
    std::int64_t rhythm_countdown = 0;
    std::size_t arp_index = 0U;
    int arp_note = -1;
    double global_lfo = 0.0;
    std::array<EffectState, 2> fx_state;
    std::atomic<float> peak{0.0F};
    float last_output_l = 0.0F;
    float last_output_r = 0.0F;
    float transition_from_l = 0.0F;
    float transition_from_r = 0.0F;
    int output_transition_remaining = 0;
    float dc_input_l = 0.0F;
    float dc_input_r = 0.0F;
    float dc_output_l = 0.0F;
    float dc_output_r = 0.0F;

    void begin_output_transition() {
        transition_from_l = last_output_l;
        transition_from_r = last_output_r;
        output_transition_remaining = std::max(output_transition_remaining, 240);
    }

    const Patch& patch() const { return kPatches[static_cast<std::size_t>(preset)]; }

    void load_patch(int index) {
        preset = index;
        controls = patch().controls;
        effects[0] = patch().fx1;
        effects[1] = patch().fx2;
        for (auto& state : fx_state) state.reset();
        begin_output_transition();
    }

    std::int64_t beat_samples(double beats) const {
        return static_cast<std::int64_t>(sample_rate * 60.0 / static_cast<double>(std::max(40, bpm)) * beats);
    }

    void release_all(bool fast = false) {
        for (auto& voice : voices) {
            if (!voice.active) continue;
            voice.gate = false;
            if (fast) voice.fast_release = true;
        }
    }

    void stop_note(int note, bool fast = false) {
        for (auto& voice : voices) {
            if (!voice.active || voice.note != note) continue;
            voice.gate = false;
            if (fast) voice.fast_release = true;
        }
    }

    void start_note(int note) {
        Voice* selected = nullptr;
        bool reused_release = false;
        for (auto& voice : voices) {
            if (voice.active && !voice.gate && voice.note == note) {
                selected = &voice;
                reused_release = true;
                break;
            }
        }
        if (selected == nullptr) {
            for (auto& voice : voices) if (!voice.active) { selected = &voice; break; }
        }
        if (selected == nullptr) {
            selected = &*std::min_element(voices.begin(), voices.end(), [](const Voice& a, const Voice& b) {
                if (a.env != b.env) return a.env < b.env;
                return a.age < b.age;
            });
        }
        selected->active = true;
        selected->gate = true;
        selected->fast_release = false;
        selected->note = note;
        if (!reused_release) {
            selected->p1 = 0.0;
            selected->p2 = 0.17;
            selected->p3 = 0.41;
            selected->p4 = 0.29;
            selected->env = std::min(selected->env, 0.05F);
            selected->low = 0.0F;
            selected->band = 0.0F;
        }
        selected->life = 0.0F;
        selected->pan = std::clamp(static_cast<float>((note % 12) - 6) / 9.0F, -0.68F, 0.68F);
        selected->noise = static_cast<std::uint32_t>(note * 747796405U + static_cast<unsigned>(voice_age));
        selected->age = ++voice_age;
    }

    void schedule_chord(const std::vector<int>& notes, bool strum) {
        release_all(true);
        scheduled.clear();
        const float motion = controls[static_cast<std::size_t>(SynthParameter::Motion)];
        const std::int64_t spacing = strum ? static_cast<std::int64_t>(sample_rate * (0.010 + 0.055 * motion)) : 0;
        for (std::size_t i = 0; i < notes.size(); ++i) scheduled.push_back({spacing * static_cast<std::int64_t>(i), notes[i]});
    }

    void set_notes(const std::vector<int>& notes) {
        held_notes = notes;
        arp_index = 0U;
        rhythm_countdown = 0;
        if (mode == PlayMode::Pad) schedule_chord(notes, false);
        else if (mode == PlayMode::Strum) schedule_chord(notes, true);
        else release_all(true);
    }

    void change_notes(const std::vector<int>& notes) {
        if (notes == held_notes) return;
        scheduled.clear();

        for (auto& voice : voices) {
            if (!voice.active || !voice.gate) continue;
            if (std::find(notes.begin(), notes.end(), voice.note) == notes.end()) {
                voice.gate = false;
                voice.fast_release = true;
            }
        }

        held_notes = notes;
        if (held_notes.empty()) {
            release_all(true);
            arp_note = -1;
            return;
        }

        if (mode == PlayMode::Pad || mode == PlayMode::Strum) {
            for (int note : held_notes) {
                const bool sounding = std::any_of(voices.begin(), voices.end(), [note](const Voice& voice) {
                    return voice.active && voice.gate && voice.note == note;
                });
                if (!sounding) start_note(note);
            }
        } else {
            if (arp_note >= 0 && std::find(held_notes.begin(), held_notes.end(), arp_note) == held_notes.end()) {
                stop_note(arp_note, true);
                arp_note = -1;
                rhythm_countdown = 0;
            }
            arp_index %= held_notes.size();
        }
    }

    void process_events() {
        for (auto it = scheduled.begin(); it != scheduled.end();) {
            if (it->samples <= 0) { start_note(it->note); it = scheduled.erase(it); }
            else { --it->samples; ++it; }
        }
        if (held_notes.empty() || (mode != PlayMode::Arp && mode != PlayMode::Pulse)) return;
        if (rhythm_countdown > 0) { --rhythm_countdown; return; }
        if (mode == PlayMode::Arp) {
            if (arp_note >= 0) stop_note(arp_note, true);
            arp_note = held_notes[arp_index % held_notes.size()];
            start_note(arp_note);
            arp_index = (arp_index + 1U) % held_notes.size();
            rhythm_countdown = beat_samples(0.5);
        } else {
            release_all(true);
            for (int note : held_notes) start_note(note);
            rhythm_countdown = beat_samples(1.0);
        }
    }

    float next_noise(Voice& voice) {
        voice.noise ^= voice.noise << 13U; voice.noise ^= voice.noise >> 17U; voice.noise ^= voice.noise << 5U;
        return static_cast<float>(voice.noise & 0xffffU) / 32767.5F - 1.0F;
    }

    float voice_sample(Voice& voice, const Patch& p, float tone, float body, float motion) {
        const float base = midi_frequency(voice.note);
        const float drift = 1.0F + (0.00025F + motion * 0.0022F) * sine(global_lfo + voice.note * 0.017);
        const float spread = controls[static_cast<std::size_t>(SynthParameter::Spread)];
        const float detune = std::pow(2.0F, ((spread - 0.5F) * 15.0F) / 1200.0F);
        const float f1 = base * drift;
        const float f2 = base * detune / drift;
        const float f3 = base * (p.engine == EngineKind::Sub ? 0.5F : 2.0F);
        const float f4 = base * ((p.engine == EngineKind::Strings || p.engine == EngineKind::Sub) ? 0.5F : 1.0F);
        float value = 0.0F;
        switch (p.engine) {
        case EngineKind::Analog:
            value = triangle(voice.p1) * 0.34F + saw(voice.p2) * 0.28F + sine(voice.p4) * (0.18F + body * 0.16F);
            value += square(voice.p1, 0.48F + motion * 0.06F) * 0.08F;
            break;
        case EngineKind::Electric: {
            const float transient = std::exp(-voice.life * (5.0F + tone * 9.0F));
            const float fm = sine(voice.p2) * (0.025F + tone * 0.16F) * transient;
            value = sine(voice.p1 + fm) * 0.64F;
            value += sine(voice.p1 * 2.0) * (0.12F + body * 0.10F) * transient;
            value += sine(voice.p3) * transient * 0.10F;
            break;
        }
        case EngineKind::Organ:
            value = sine(voice.p1) * 0.50F + sine(voice.p2) * (0.18F + body * 0.12F);
            value += sine(voice.p3) * 0.12F + sine(voice.p1 * 4.0) * tone * 0.05F;
            break;
        case EngineKind::Strings:
            value = triangle(voice.p1) * 0.34F + saw(voice.p2) * 0.24F + triangle(voice.p3) * 0.22F;
            value += sine(voice.p4) * body * 0.12F;
            break;
        case EngineKind::Choir: {
            const float vowel = 0.5F + 0.5F * sine(global_lfo * (0.18 + motion * 0.54));
            value = triangle(voice.p1) * (0.46F + vowel * 0.10F) + sine(voice.p2) * 0.22F;
            value += sine(voice.p3 + (vowel - 0.5F) * 0.18F) * (0.10F + body * 0.10F);
            value += next_noise(voice) * 0.008F * motion;
            break;
        }
        case EngineKind::Pluck: {
            const float decay = std::exp(-voice.life * (1.6F + tone * 5.8F));
            value = (triangle(voice.p1) * 0.58F + sine(voice.p2 * 2.0) * 0.22F + next_noise(voice) * 0.06F) * decay;
            break;
        }
        case EngineKind::Glass: {
            const float index = (0.5F + tone * 3.4F) * std::exp(-voice.life * (0.8F + body * 2.2F));
            value = sine(voice.p1 + sine(voice.p2) * index * 0.08F) * 0.76F + sine(voice.p3) * 0.12F;
            break;
        }
        case EngineKind::Doom:
            value = square(voice.p1, 0.47F) * 0.30F + saw(voice.p2) * 0.22F + sine(voice.p4) * (0.16F + body * 0.14F);
            value = soft_clip(value, 1.25F + body * 2.2F);
            break;
        case EngineKind::Reed: {
            const float width = 0.34F + 0.10F * sine(global_lfo * (0.35F + motion * 0.55F));
            value = square(voice.p1, width) * 0.26F + sine(voice.p2) * 0.48F + triangle(voice.p3) * 0.14F;
            break;
        }
        case EngineKind::Sub:
            value = sine(voice.p1) * 0.62F + sine(voice.p4) * 0.20F + triangle(voice.p3) * body * 0.08F;
            break;
        }
        voice.p1 += static_cast<double>(f1 / static_cast<float>(sample_rate));
        voice.p2 += static_cast<double>((f2 * (p.engine == EngineKind::Organ ? 2.0F : 1.003F)) / static_cast<float>(sample_rate));
        voice.p3 += static_cast<double>((f3 * (p.engine == EngineKind::Electric ? 3.0F : 1.0F)) / static_cast<float>(sample_rate));
        voice.p4 += static_cast<double>(f4 / static_cast<float>(sample_rate));
        voice.p1 -= std::floor(voice.p1); voice.p2 -= std::floor(voice.p2);
        voice.p3 -= std::floor(voice.p3); voice.p4 -= std::floor(voice.p4);
        return value;
    }

    std::pair<float,float> process_effect(int slot, float left, float right) {
        const EffectSettings settings = effects[static_cast<std::size_t>(slot)];
        if (settings.type == EffectType::Off || settings.amount <= kEpsilon) return {left, right};
        EffectState& state = fx_state[static_cast<std::size_t>(slot)];
        const float amount = clamp01(settings.amount);
        const float colour = clamp01(settings.colour);
        const float wet = amount;
        float out_l = left;
        float out_r = right;
        switch (settings.type) {
        case EffectType::Off: break;
        case EffectType::Chorus: {
            const float phase = 0.5F + 0.5F * sine(state.lfo);
            const std::size_t delay = 90U + static_cast<std::size_t>((120.0F + colour * 520.0F) * phase);
            const std::size_t read = (state.position + state.left.size() - std::min(delay, state.left.size() - 1U)) % state.left.size();
            const float dl = state.read_left(read); const float dr = state.read_right((read + 67U) % state.right.size());
            state.left[state.position] = left; state.right[state.position] = right;
            out_l = left * (1.0F - wet * 0.45F) + dr * wet * 0.55F;
            out_r = right * (1.0F - wet * 0.45F) + dl * wet * 0.55F;
            state.lfo += (0.08 + colour * 0.72) / sample_rate;
            break;
        }
        case EffectType::Phaser: {
            const float coefficient = 0.08F + (0.82F * (0.5F + 0.5F * sine(state.lfo)));
            const float y_l = -coefficient * left + state.ap_l1 + coefficient * state.ap_l2;
            state.ap_l1 = left; state.ap_l2 = y_l;
            const float y_r = -coefficient * right + state.ap_r1 + coefficient * state.ap_r2;
            state.ap_r1 = right; state.ap_r2 = y_r;
            out_l = left * (1.0F - wet) + y_l * wet;
            out_r = right * (1.0F - wet) + y_r * wet;
            state.lfo += (0.05 + colour * 1.6) / sample_rate;
            break;
        }
        case EffectType::Tremolo: {
            const float mod = 1.0F - wet * (0.25F + 0.70F * (0.5F + 0.5F * sine(state.lfo)));
            out_l = left * mod; out_r = right * mod;
            state.lfo += (0.3 + colour * 8.0) / sample_rate;
            break;
        }
        case EffectType::Drive: {
            const float gain = 1.0F + wet * (2.0F + colour * 12.0F);
            const float compensation = 0.78F - wet * 0.10F;
            const float driven_l = soft_clip(left, gain) * compensation;
            const float driven_r = soft_clip(right, gain) * compensation;
            out_l = left * (1.0F - wet) + driven_l * wet;
            out_r = right * (1.0F - wet) + driven_r * wet;
            break;
        }
        case EffectType::Crusher: {
            const int hold = 1 + static_cast<int>((1.0F - colour) * 34.0F);
            if (state.hold <= 0) {
                const float steps = 16.0F + (1.0F - wet) * 496.0F;
                state.held_l = std::round(left * steps) / steps;
                state.held_r = std::round(right * steps) / steps;
                state.hold = hold;
            }
            --state.hold;
            out_l = left * (1.0F - wet) + state.held_l * wet;
            out_r = right * (1.0F - wet) + state.held_r * wet;
            break;
        }
        case EffectType::Delay: {
            const std::size_t length = std::max<std::size_t>(1U, static_cast<std::size_t>(sample_rate * (0.08 + colour * 0.82)));
            const std::size_t read = (state.position + state.left.size() - std::min(length, state.left.size() - 1U)) % state.left.size();
            const float dl = state.read_left(read); const float dr = state.read_right(read);
            const float feedback = 0.14F + amount * 0.48F;
            state.left[state.position] = std::clamp(left * 0.72F + dr * feedback, -1.25F, 1.25F);
            state.right[state.position] = std::clamp(right * 0.72F + dl * feedback, -1.25F, 1.25F);
            out_l = left * (1.0F - wet * 0.22F) + dl * wet * 0.48F;
            out_r = right * (1.0F - wet * 0.22F) + dr * wet * 0.48F;
            break;
        }
        case EffectType::Reverb: {
            const std::size_t length = std::max<std::size_t>(1U, static_cast<std::size_t>(sample_rate * (0.16 + colour * 0.70)));
            const std::size_t read = (state.position + state.left.size() - std::min(length, state.left.size() - 1U)) % state.left.size();
            const float dl = state.read_left(read); const float dr = state.read_right((read + 953U) % state.right.size());
            const float feedback = 0.42F + colour * 0.24F;
            state.left[state.position] = std::clamp(left * 0.68F + dr * feedback, -1.20F, 1.20F);
            state.right[state.position] = std::clamp(right * 0.68F + dl * feedback, -1.20F, 1.20F);
            out_l = left * (1.0F - wet * 0.24F) + (dl + dr * 0.28F) * wet * 0.42F;
            out_r = right * (1.0F - wet * 0.24F) + (dr + dl * 0.28F) * wet * 0.42F;
            break;
        }
        case EffectType::Count: break;
        }
        if (settings.type == EffectType::Chorus || settings.type == EffectType::Delay || settings.type == EffectType::Reverb) {
            state.valid_samples = std::min(state.valid_samples + 1U, state.left.size());
        }
        state.position = (state.position + 1U) % state.left.size();
        if (state.lfo >= 1.0) state.lfo -= 1.0;
        return {out_l, out_r};
    }

    void render_locked(float* out, std::size_t frames) {
        const Patch p = patch();
        const float attack_control = controls[static_cast<std::size_t>(SynthParameter::Attack)];
        const float release_control = controls[static_cast<std::size_t>(SynthParameter::Release)];
        const float tone = controls[static_cast<std::size_t>(SynthParameter::Tone)];
        const float body = controls[static_cast<std::size_t>(SynthParameter::Body)];
        const float motion = controls[static_cast<std::size_t>(SynthParameter::Motion)];
        const float spread = controls[static_cast<std::size_t>(SynthParameter::Spread)];
        const float master = controls[static_cast<std::size_t>(SynthParameter::Master)];
        const float attack_seconds = 0.002F + attack_control * attack_control * 1.8F;
        const float release_seconds = 0.025F + release_control * release_control * 4.5F;
        const float attack_step = 1.0F / std::max(1.0F, attack_seconds * static_cast<float>(sample_rate));
        const float release_step = 1.0F / std::max(1.0F, release_seconds * static_cast<float>(sample_rate));
        const float fast_release_step = 1.0F / std::max(1.0F, 0.012F * static_cast<float>(sample_rate));
        const float cutoff_hz = 90.0F + tone * tone * 14500.0F;
        const float filter = 2.0F * std::sin(static_cast<float>(kPi) * std::min(0.45F, cutoff_hz / static_cast<float>(sample_rate)));
        float block_peak = 0.0F;
        for (std::size_t frame = 0; frame < frames; ++frame) {
            process_events();
            float left = 0.0F; float right = 0.0F; int active = 0;
            for (auto& voice : voices) {
                if (!voice.active) continue;
                voice.life += 1.0F / static_cast<float>(sample_rate);
                const float decay_target = p.sustain + (1.0F - p.sustain) * std::exp(-voice.life / std::max(0.04F, p.decay));
                if (voice.gate) {
                    if (voice.env < decay_target) voice.env = std::min(decay_target, voice.env + attack_step);
                    else voice.env += (decay_target - voice.env) * 0.0008F;
                } else {
                    const float step = voice.fast_release ? fast_release_step : release_step;
                    voice.env = std::max(0.0F, voice.env - step);
                }
                if (!voice.gate && voice.env <= kEpsilon) { voice.active = false; voice.fast_release = false; continue; }
                ++active;
                float sample = voice_sample(voice, p, tone, body, motion) * voice.env * 0.24F * p.level;
                voice.low += filter * voice.band;
                const float high = sample - voice.low - 0.32F * voice.band;
                voice.band += filter * high;
                sample = voice.low + voice.band * (0.22F + body * 0.42F) + high * tone * 0.05F;
                const float pan = std::clamp(voice.pan * (0.20F + spread * 0.72F), -0.86F, 0.86F);
                left += sample * std::sqrt(0.5F * (1.0F - pan));
                right += sample * std::sqrt(0.5F * (1.0F + pan));
            }
            if (active > 1) {
                const float normalise = 1.0F / std::sqrt(0.62F * static_cast<float>(active));
                left *= normalise; right *= normalise;
            }
            auto first = process_effect(0, left, right);
            auto second = process_effect(1, first.first, first.second);
            const float gain = 0.36F + master * 0.88F;
            const float driven_l = second.first * gain;
            const float driven_r = second.second * gain;
            const float blocked_l = driven_l - dc_input_l + 0.9995F * dc_output_l;
            const float blocked_r = driven_r - dc_input_r + 0.9995F * dc_output_r;
            dc_input_l = driven_l; dc_input_r = driven_r;
            dc_output_l = blocked_l; dc_output_r = blocked_r;
            const float target_l = std::clamp(soft_clip(blocked_l, 1.18F), -0.98F, 0.98F);
            const float target_r = std::clamp(soft_clip(blocked_r, 1.18F), -0.98F, 0.98F);
            if (output_transition_remaining > 0) {
                const float mix = 1.0F - static_cast<float>(output_transition_remaining) / 240.0F;
                left = transition_from_l + (target_l - transition_from_l) * mix;
                right = transition_from_r + (target_r - transition_from_r) * mix;
                --output_transition_remaining;
            } else {
                left = target_l; right = target_r;
            }
            last_output_l = left; last_output_r = right;
            block_peak = std::max(block_peak, std::max(std::abs(left), std::abs(right)));
            out[frame * 2U] = left;
            out[frame * 2U + 1U] = right;
            global_lfo += (0.03 + motion * 0.62) / sample_rate;
        }
        peak.store(peak.load(std::memory_order_relaxed) * 0.86F + block_peak * 0.14F, std::memory_order_relaxed);
    }
};

SynthEngine::SynthEngine(double sample_rate) : impl_(new Impl(sample_rate)) {}
SynthEngine::~SynthEngine() { delete impl_; }

void SynthEngine::set_preset(int index) {
    std::scoped_lock lock(impl_->mutex);
    const int count = static_cast<int>(kPatches.size());
    index %= count; if (index < 0) index += count;
    impl_->load_patch(index);
}
int SynthEngine::preset_index() const { std::scoped_lock lock(impl_->mutex); return impl_->preset; }
PresetInfo SynthEngine::preset_info() const {
    std::scoped_lock lock(impl_->mutex); const Patch& p = impl_->patch(); return {p.name, p.character, p.engine_name};
}
VoicingProfile SynthEngine::voicing_profile() const { std::scoped_lock lock(impl_->mutex); return impl_->patch().voicing; }
int SynthEngine::preset_count() { return static_cast<int>(kPatches.size()); }

void SynthEngine::set_parameter(SynthParameter parameter, float value) {
    std::scoped_lock lock(impl_->mutex); impl_->controls[static_cast<std::size_t>(parameter)] = clamp01(value);
}
float SynthEngine::parameter(SynthParameter parameter) const {
    std::scoped_lock lock(impl_->mutex); return impl_->controls[static_cast<std::size_t>(parameter)];
}
void SynthEngine::set_effect(int slot, EffectSettings settings) {
    if (slot < 0 || slot >= 2) return;
    std::scoped_lock lock(impl_->mutex);
    settings.amount = clamp01(settings.amount); settings.colour = clamp01(settings.colour);
    const std::size_t index = static_cast<std::size_t>(slot);
    const EffectSettings previous = impl_->effects[index];
    const bool reset_state = previous.type != settings.type ||
        (previous.amount <= kEpsilon && settings.amount > kEpsilon);
    const bool changed = previous.type != settings.type ||
        std::abs(previous.amount - settings.amount) > 0.001F ||
        std::abs(previous.colour - settings.colour) > 0.001F;
    impl_->effects[index] = settings;
    if (reset_state) impl_->fx_state[index].reset();
    if (changed) impl_->begin_output_transition();
}
EffectSettings SynthEngine::effect(int slot) const {
    if (slot < 0 || slot >= 2) return {};
    std::scoped_lock lock(impl_->mutex); return impl_->effects[static_cast<std::size_t>(slot)];
}
int SynthEngine::effect_count() { return static_cast<int>(EffectType::Count); }
std::string SynthEngine::effect_name(EffectType type) {
    switch (type) {
    case EffectType::Off: return "OFF"; case EffectType::Chorus: return "CHORUS"; case EffectType::Phaser: return "PHASER";
    case EffectType::Tremolo: return "TREMOLO"; case EffectType::Drive: return "DRIVE"; case EffectType::Crusher: return "CRUSHER";
    case EffectType::Delay: return "DELAY"; case EffectType::Reverb: return "REVERB"; case EffectType::Count: break;
    }
    return "OFF";
}
void SynthEngine::set_mode(PlayMode value) { std::scoped_lock lock(impl_->mutex); impl_->mode = value; if (!impl_->held_notes.empty()) impl_->set_notes(impl_->held_notes); }
void SynthEngine::set_bpm(int value) { std::scoped_lock lock(impl_->mutex); impl_->bpm = std::clamp(value, 40, 240); }
void SynthEngine::set_latch(bool value) { std::scoped_lock lock(impl_->mutex); impl_->latch = value; if (!value && impl_->held_notes.empty()) impl_->release_all(); }
void SynthEngine::play_chord(const std::vector<int>& notes) { std::scoped_lock lock(impl_->mutex); impl_->set_notes(notes); }
void SynthEngine::change_chord(const std::vector<int>& notes) { std::scoped_lock lock(impl_->mutex); impl_->change_notes(notes); }
void SynthEngine::release_chord() {
    std::scoped_lock lock(impl_->mutex); if (impl_->latch) return; impl_->held_notes.clear(); impl_->scheduled.clear(); impl_->release_all(); impl_->arp_note = -1;
}
void SynthEngine::all_notes_off() {
    std::scoped_lock lock(impl_->mutex); impl_->held_notes.clear(); impl_->scheduled.clear();
    for (auto& voice : impl_->voices) { voice.active = false; voice.gate = false; voice.fast_release = false; voice.env = 0.0F; }
    for (auto& state : impl_->fx_state) state.reset();
    impl_->dc_input_l = impl_->dc_input_r = 0.0F;
    impl_->dc_output_l = impl_->dc_output_r = 0.0F;
    impl_->last_output_l = impl_->last_output_r = 0.0F;
    impl_->begin_output_transition();
}
float SynthEngine::output_peak() const { return impl_->peak.load(std::memory_order_relaxed); }
void SynthEngine::render(float* interleaved_stereo, std::size_t frames) { std::scoped_lock lock(impl_->mutex); impl_->render_locked(interleaved_stereo, frames); }
std::vector<float> SynthEngine::render_copy(std::size_t frames) { std::vector<float> result(frames * 2U, 0.0F); render(result.data(), frames); return result; }

bool write_wav(const std::string& path, const std::vector<float>& stereo, int sample_rate) {
    std::ofstream out(path, std::ios::binary); if (!out) return false;
    const std::uint32_t bytes = static_cast<std::uint32_t>(stereo.size() * sizeof(std::int16_t));
    out.write("RIFF", 4); write_u32(out, 36U + bytes); out.write("WAVEfmt ", 8); write_u32(out, 16U); write_u16(out, 1U);
    write_u16(out, 2U); write_u32(out, static_cast<std::uint32_t>(sample_rate)); write_u32(out, static_cast<std::uint32_t>(sample_rate * 4));
    write_u16(out, 4U); write_u16(out, 16U); out.write("data", 4); write_u32(out, bytes);
    for (float sample : stereo) { const float clipped = std::clamp(sample, -1.0F, 1.0F); write_u16(out, static_cast<std::uint16_t>(static_cast<std::int16_t>(std::lrint(clipped * 32767.0F)))); }
    return static_cast<bool>(out);
}

} // namespace brkchrd
