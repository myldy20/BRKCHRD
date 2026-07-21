from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return text.replace(old, new, 1)


synth_path = ROOT / "src" / "synth.cpp"
synth = synth_path.read_text()

synth = replace_once(
    synth,
    "float soft_clip(float value, float drive) { const float d = std::max(1.0F, drive); return std::tanh(value * d) / std::tanh(d); }",
    "float soft_clip(float value, float drive) {\n"
    "    const float d = std::max(1.0F, drive);\n"
    "    return std::clamp(std::tanh(value * d) / std::tanh(d), -1.0F, 1.0F);\n"
    "}",
    "bounded soft clip",
)

synth = replace_once(
    synth,
    "    double p3 = 0.0;\n    float env = 0.0F;",
    "    double p3 = 0.0;\n    double p4 = 0.0;\n    float env = 0.0F;",
    "fourth oscillator phase",
)

synth = replace_once(
    synth,
    "            selected->p3 = 0.41;\n            selected->env = std::min(selected->env, 0.05F);",
    "            selected->p3 = 0.41;\n            selected->p4 = 0.29;\n            selected->env = std::min(selected->env, 0.05F);",
    "reset fourth phase",
)

synth = replace_once(
    synth,
    "    std::array<EffectState, 2> fx_state;\n    std::atomic<float> peak{0.0F};\n\n    const Patch& patch() const",
    "    std::array<EffectState, 2> fx_state;\n"
    "    std::atomic<float> peak{0.0F};\n"
    "    float last_output_l = 0.0F;\n"
    "    float last_output_r = 0.0F;\n"
    "    float transition_from_l = 0.0F;\n"
    "    float transition_from_r = 0.0F;\n"
    "    int output_transition_remaining = 0;\n"
    "    float dc_input_l = 0.0F;\n"
    "    float dc_input_r = 0.0F;\n"
    "    float dc_output_l = 0.0F;\n"
    "    float dc_output_r = 0.0F;\n\n"
    "    void begin_output_transition() {\n"
    "        transition_from_l = last_output_l;\n"
    "        transition_from_r = last_output_r;\n"
    "        output_transition_remaining = std::max(output_transition_remaining, 240);\n"
    "    }\n\n"
    "    const Patch& patch() const",
    "output safety state",
)

synth = replace_once(
    synth,
    "        for (auto& state : fx_state) state.reset();\n    }",
    "        for (auto& state : fx_state) state.reset();\n"
    "        begin_output_transition();\n"
    "    }",
    "preset transition",
)

synth = replace_once(
    synth,
    "        const float f3 = base * (p.engine == EngineKind::Sub ? 0.5F : 2.0F);\n        float value = 0.0F;",
    "        const float f3 = base * (p.engine == EngineKind::Sub ? 0.5F : 2.0F);\n"
    "        const float f4 = base * ((p.engine == EngineKind::Strings || p.engine == EngineKind::Sub) ? 0.5F : 1.0F);\n"
    "        float value = 0.0F;",
    "continuous auxiliary frequency",
)

synth = replace_once(
    synth,
    "            value = triangle(voice.p1) * 0.34F + saw(voice.p2) * 0.28F + sine(voice.p3 * 0.5) * (0.18F + body * 0.16F);",
    "            value = triangle(voice.p1) * 0.34F + saw(voice.p2) * 0.28F + sine(voice.p4) * (0.18F + body * 0.16F);",
    "analog half-wave removal",
)

synth = replace_once(
    synth,
    "            value += sine(voice.p1 * 0.5) * body * 0.12F;",
    "            value += sine(voice.p4) * body * 0.12F;",
    "strings sub oscillator",
)

synth = replace_once(
    synth,
    "            value += sine(voice.p3 * (1.5 + vowel * 1.5)) * (0.10F + body * 0.10F);",
    "            value += sine(voice.p3 + (vowel - 0.5F) * 0.18F) * (0.10F + body * 0.10F);",
    "continuous choir formant",
)

synth = replace_once(
    synth,
    "            value = square(voice.p1, 0.47F) * 0.30F + saw(voice.p2) * 0.22F + sine(voice.p3 * 0.5) * (0.16F + body * 0.14F);",
    "            value = square(voice.p1, 0.47F) * 0.30F + saw(voice.p2) * 0.22F + sine(voice.p4) * (0.16F + body * 0.14F);",
    "doom half-wave removal",
)

synth = replace_once(
    synth,
    "            value = sine(voice.p1) * 0.62F + sine(voice.p2 * 0.5) * 0.20F + triangle(voice.p3 * 0.5) * body * 0.08F;",
    "            value = sine(voice.p1) * 0.62F + sine(voice.p4) * 0.20F + triangle(voice.p3) * body * 0.08F;",
    "continuous sub oscillators",
)

synth = replace_once(
    synth,
    "        voice.p3 += static_cast<double>((f3 * (p.engine == EngineKind::Electric ? 3.0F : 1.0F)) / static_cast<float>(sample_rate));\n        voice.p1 -= std::floor(voice.p1); voice.p2 -= std::floor(voice.p2); voice.p3 -= std::floor(voice.p3);",
    "        voice.p3 += static_cast<double>((f3 * (p.engine == EngineKind::Electric ? 3.0F : 1.0F)) / static_cast<float>(sample_rate));\n"
    "        voice.p4 += static_cast<double>(f4 / static_cast<float>(sample_rate));\n"
    "        voice.p1 -= std::floor(voice.p1); voice.p2 -= std::floor(voice.p2);\n"
    "        voice.p3 -= std::floor(voice.p3); voice.p4 -= std::floor(voice.p4);",
    "advance fourth oscillator",
)

synth = replace_once(
    synth,
    "            auto first = process_effect(0, left, right);\n"
    "            auto second = process_effect(1, first.first, first.second);\n"
    "            left = soft_clip(second.first * (0.36F + master * 0.88F), 1.18F);\n"
    "            right = soft_clip(second.second * (0.36F + master * 0.88F), 1.18F);",
    "            auto first = process_effect(0, left, right);\n"
    "            auto second = process_effect(1, first.first, first.second);\n"
    "            const float gain = 0.36F + master * 0.88F;\n"
    "            const float driven_l = second.first * gain;\n"
    "            const float driven_r = second.second * gain;\n"
    "            const float blocked_l = driven_l - dc_input_l + 0.9995F * dc_output_l;\n"
    "            const float blocked_r = driven_r - dc_input_r + 0.9995F * dc_output_r;\n"
    "            dc_input_l = driven_l; dc_input_r = driven_r;\n"
    "            dc_output_l = blocked_l; dc_output_r = blocked_r;\n"
    "            const float target_l = std::clamp(soft_clip(blocked_l, 1.18F), -0.98F, 0.98F);\n"
    "            const float target_r = std::clamp(soft_clip(blocked_r, 1.18F), -0.98F, 0.98F);\n"
    "            if (output_transition_remaining > 0) {\n"
    "                const float mix = 1.0F - static_cast<float>(output_transition_remaining) / 240.0F;\n"
    "                left = transition_from_l + (target_l - transition_from_l) * mix;\n"
    "                right = transition_from_r + (target_r - transition_from_r) * mix;\n"
    "                --output_transition_remaining;\n"
    "            } else {\n"
    "                left = target_l; right = target_r;\n"
    "            }\n"
    "            last_output_l = left; last_output_r = right;",
    "dc block limiter and transition",
)

synth = replace_once(
    synth,
    "            global_lfo += (0.03 + motion * 0.62) / sample_rate;\n            if (global_lfo >= 1.0) global_lfo -= 1.0;",
    "            global_lfo += (0.03 + motion * 0.62) / sample_rate;",
    "continuous global lfo",
)

synth = replace_once(
    synth,
    "    const bool reset_state = previous.type != settings.type ||\n        (previous.amount <= kEpsilon && settings.amount > kEpsilon);\n    impl_->effects[index] = settings;\n    if (reset_state) impl_->fx_state[index].reset();",
    "    const bool reset_state = previous.type != settings.type ||\n"
    "        (previous.amount <= kEpsilon && settings.amount > kEpsilon);\n"
    "    const bool changed = previous.type != settings.type ||\n"
    "        std::abs(previous.amount - settings.amount) > 0.001F ||\n"
    "        std::abs(previous.colour - settings.colour) > 0.001F;\n"
    "    impl_->effects[index] = settings;\n"
    "    if (reset_state) impl_->fx_state[index].reset();\n"
    "    if (changed) impl_->begin_output_transition();",
    "effect transition trigger",
)

synth = replace_once(
    synth,
    "    for (auto& voice : impl_->voices) { voice.active = false; voice.gate = false; voice.fast_release = false; voice.env = 0.0F; }\n}",
    "    for (auto& voice : impl_->voices) { voice.active = false; voice.gate = false; voice.fast_release = false; voice.env = 0.0F; }\n"
    "    for (auto& state : impl_->fx_state) state.reset();\n"
    "    impl_->dc_input_l = impl_->dc_input_r = 0.0F;\n"
    "    impl_->dc_output_l = impl_->dc_output_r = 0.0F;\n"
    "    impl_->last_output_l = impl_->last_output_r = 0.0F;\n"
    "    impl_->begin_output_transition();\n"
    "}",
    "panic clears full signal path",
)

synth_path.write_text(synth)

sdl_path = ROOT / "src" / "sdl_main.cpp"
sdl = sdl_path.read_text()

sdl = replace_once(
    sdl,
    "#include <algorithm>\n#include <array>",
    "#include <algorithm>\n#include <array>\n#include <atomic>",
    "atomic include",
)

sdl = replace_once(
    sdl,
    "void audio_callback(void* userdata, Uint8* bytes, int count) {\n"
    "    auto* synth = static_cast<SynthEngine*>(userdata);\n"
    "    synth->render(reinterpret_cast<float*>(bytes), static_cast<std::size_t>(count) / (sizeof(float) * 2U));\n"
    "}",
    "struct AudioCallbackState {\n"
    "    SynthEngine* synth = nullptr;\n"
    "    Uint64 frequency = 1U;\n"
    "    int sample_rate = kRate;\n"
    "    std::atomic<Uint64> callbacks{0U};\n"
    "    std::atomic<Uint64> overruns{0U};\n"
    "    std::atomic<Uint64> maximum_ticks{0U};\n"
    "};\n\n"
    "void audio_callback(void* userdata, Uint8* bytes, int count) {\n"
    "    auto* state = static_cast<AudioCallbackState*>(userdata);\n"
    "    const Uint64 begin = SDL_GetPerformanceCounter();\n"
    "    const std::size_t frames = static_cast<std::size_t>(count) / (sizeof(float) * 2U);\n"
    "    state->synth->render(reinterpret_cast<float*>(bytes), frames);\n"
    "    const Uint64 elapsed = SDL_GetPerformanceCounter() - begin;\n"
    "    const Uint64 deadline = static_cast<Uint64>(frames) * state->frequency /\n"
    "        static_cast<Uint64>(std::max(1, state->sample_rate));\n"
    "    state->callbacks.fetch_add(1U, std::memory_order_relaxed);\n"
    "    if (elapsed > deadline) state->overruns.fetch_add(1U, std::memory_order_relaxed);\n"
    "    Uint64 previous = state->maximum_ticks.load(std::memory_order_relaxed);\n"
    "    while (elapsed > previous && !state->maximum_ticks.compare_exchange_weak(\n"
    "        previous, elapsed, std::memory_order_relaxed, std::memory_order_relaxed)) {}\n"
    "}",
    "audio timing callback",
)

sdl = replace_once(
    sdl,
    "    SynthEngine synth(kRate); PerformanceState state; UiState ui;\n    load_config(state, ui, synth); synth.set_mode(state.mode); synth.set_bpm(state.bpm); synth.set_latch(false);\n\n    SDL_AudioSpec desired{};",
    "    SynthEngine synth(kRate); PerformanceState state; UiState ui;\n"
    "    load_config(state, ui, synth); synth.set_mode(state.mode); synth.set_bpm(state.bpm); synth.set_latch(false);\n"
    "    AudioCallbackState audio_state; audio_state.synth = &synth;\n"
    "    audio_state.frequency = SDL_GetPerformanceFrequency();\n\n"
    "    SDL_AudioSpec desired{};",
    "audio timing state",
)

sdl = replace_once(
    sdl,
    "    desired.samples = kFrames; desired.callback = audio_callback; desired.userdata = &synth;",
    "    desired.samples = kFrames; desired.callback = audio_callback; desired.userdata = &audio_state;",
    "callback userdata",
)

sdl = replace_once(
    sdl,
    "    std::cerr << \"audio backend=\" << (SDL_GetCurrentAudioDriver() ? SDL_GetCurrentAudioDriver() : \"unknown\")\n               << \" rate=\" << obtained.freq << \" frames=\" << obtained.samples\n               << \" channels=\" << static_cast<int>(obtained.channels) << '\\n';\n    SDL_PauseAudioDevice(audio, 0);",
    "    std::cerr << \"audio backend=\" << (SDL_GetCurrentAudioDriver() ? SDL_GetCurrentAudioDriver() : \"unknown\")\n"
    "               << \" rate=\" << obtained.freq << \" frames=\" << obtained.samples\n"
    "               << \" channels=\" << static_cast<int>(obtained.channels) << '\\n';\n"
    "    audio_state.sample_rate = obtained.freq;\n"
    "    SDL_PauseAudioDevice(audio, 0);",
    "obtained sample rate",
)

sdl = replace_once(
    sdl,
    "    bool running = true; int raw_log_budget = 96;",
    "    bool running = true; int raw_log_budget = 96;\n"
    "    Uint32 next_audio_report = SDL_GetTicks() + 5000U;",
    "audio report timer",
)

sdl = replace_once(
    sdl,
    "        repeat_edit(ui, state, input, synth, current, previous);\n        if (SDL_GetTicks() >= ui.toast_until) ui.toast.clear();\n        draw_ui(renderer, state, ui, input, current, synth);",
    "        repeat_edit(ui, state, input, synth, current, previous);\n"
    "        if (SDL_GetTicks() >= ui.toast_until) ui.toast.clear();\n"
    "        if (SDL_GetTicks() >= next_audio_report) {\n"
    "            const Uint64 callbacks = audio_state.callbacks.exchange(0U, std::memory_order_relaxed);\n"
    "            const Uint64 overruns = audio_state.overruns.exchange(0U, std::memory_order_relaxed);\n"
    "            const Uint64 maximum = audio_state.maximum_ticks.exchange(0U, std::memory_order_relaxed);\n"
    "            const double maximum_ms = 1000.0 * static_cast<double>(maximum) /\n"
    "                static_cast<double>(std::max<Uint64>(1U, audio_state.frequency));\n"
    "            const double deadline_ms = 1000.0 * static_cast<double>(obtained.samples) /\n"
    "                static_cast<double>(std::max(1, obtained.freq));\n"
    "            std::cerr << \"audio timing callbacks=\" << callbacks << \" overruns=\" << overruns\n"
    "                      << \" max_ms=\" << maximum_ms << \" deadline_ms=\" << deadline_ms << '\\n';\n"
    "            next_audio_report = SDL_GetTicks() + 5000U;\n"
    "        }\n"
    "        draw_ui(renderer, state, ui, input, current, synth);",
    "periodic audio report",
)

sdl_path.write_text(sdl)
