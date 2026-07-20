from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return text.replace(old, new, 1)


def replace_between(text: str, start: str, end: str, replacement: str, label: str) -> str:
    first = text.find(start)
    if first < 0:
        raise RuntimeError(f"{label}: start marker not found")
    last = text.find(end, first)
    if last < 0:
        raise RuntimeError(f"{label}: end marker not found")
    return text[:first] + replacement + text[last:]


synth_path = ROOT / "src/synth.cpp"
synth = synth_path.read_text()
synth = replace_once(
    synth,
    "    float held_l = 0.0F;\n    float held_r = 0.0F;\n};",
    "    float held_l = 0.0F;\n    float held_r = 0.0F;\n\n"
    "    void reset() {\n"
    "        std::fill(left.begin(), left.end(), 0.0F);\n"
    "        std::fill(right.begin(), right.end(), 0.0F);\n"
    "        position = 0U;\n"
    "        lfo = 0.0;\n"
    "        ap_l1 = ap_l2 = ap_r1 = ap_r2 = 0.0F;\n"
    "        hold = 0;\n"
    "        held_l = held_r = 0.0F;\n"
    "    }\n"
    "};",
    "effect state reset",
)
synth = replace_once(
    synth,
    "        effects[0] = patch().fx1;\n        effects[1] = patch().fx2;\n    }",
    "        effects[0] = patch().fx1;\n        effects[1] = patch().fx2;\n"
    "        for (auto& state : fx_state) state.reset();\n"
    "    }",
    "preset effect reset",
)
synth = replace_once(
    synth,
    "void SynthEngine::set_effect(int slot, EffectSettings settings) {\n"
    "    if (slot < 0 || slot >= 2) return;\n"
    "    std::scoped_lock lock(impl_->mutex);\n"
    "    settings.amount = clamp01(settings.amount); settings.colour = clamp01(settings.colour);\n"
    "    impl_->effects[static_cast<std::size_t>(slot)] = settings;\n"
    "}",
    "void SynthEngine::set_effect(int slot, EffectSettings settings) {\n"
    "    if (slot < 0 || slot >= 2) return;\n"
    "    std::scoped_lock lock(impl_->mutex);\n"
    "    settings.amount = clamp01(settings.amount); settings.colour = clamp01(settings.colour);\n"
    "    const std::size_t index = static_cast<std::size_t>(slot);\n"
    "    const EffectSettings previous = impl_->effects[index];\n"
    "    const bool reset_state = previous.type != settings.type ||\n"
    "        (previous.amount <= kEpsilon && settings.amount > kEpsilon);\n"
    "    impl_->effects[index] = settings;\n"
    "    if (reset_state) impl_->fx_state[index].reset();\n"
    "}",
    "set effect reset",
)
synth_path.write_text(synth)

music_path = ROOT / "src/music.cpp"
music = music_path.read_text()
music = replace_once(
    music,
    "        const auto fifth = std::find_if(intervals.begin(), intervals.end(), [](int i) { return wrap_pc(i) == 7; });\n"
    "        if (fifth != intervals.end()) bass.push_back(7);",
    "        const auto fifth = std::find_if(intervals.begin(), intervals.end(), [](int i) {\n"
    "            const int pitch_class = wrap_pc(i);\n"
    "            return pitch_class == 7 || pitch_class == 6;\n"
    "        });\n"
    "        if (fifth != intervals.end()) bass.push_back(wrap_pc(*fifth));",
    "bass diminished fifth",
)
music_path.write_text(music)

sdl_path = ROOT / "src/sdl_main.cpp"
sdl = sdl_path.read_text()
sdl = replace_once(sdl, '#include "brkchrd/music.hpp"\n#include "brkchrd/synth.hpp"',
                   '#include "brkchrd/config.hpp"\n#include "brkchrd/input_safety.hpp"\n#include "brkchrd/music.hpp"\n#include "brkchrd/synth.hpp"',
                   "sdl safety includes")
sdl = replace_once(sdl, "#include <optional>\n#include <string>", "#include <optional>\n#include <sstream>\n#include <string>", "sstream include")

sdl = replace_between(
    sdl,
    "struct InputState {",
    "struct UiState {",
    "struct InputState {\n"
    "    std::array<bool, 4> face{};\n"
    "    FacePressState face_order;\n"
    "    bool up = false, down = false, left = false, right = false;\n"
    "    bool front_left = false, front_right = false;\n"
    "    bool l1 = false, l2 = false, r1 = false, r2 = false;\n"
    "    bool left_trigger = false, right_trigger = false;\n"
    "    bool start = false, select = false;\n"
    "    bool start_long = false;\n"
    "    Uint32 start_since = 0U;\n"
    "    std::optional<Face> active_face;\n"
    "};\n\n",
    "input state",
)

new_load = r'''void load_config(PerformanceState& p, UiState& ui, SynthEngine& synth) {
    const ConfigReadResult config = read_numeric_config(config_path());
    for (const std::string& warning : config.warnings) std::cerr << "config: " << warning << '\n';

    std::array<float, static_cast<std::size_t>(SynthParameter::Count)> params{};
    std::array<bool, static_cast<std::size_t>(SynthParameter::Count)> param_seen{};
    std::array<EffectSettings, 2> effects{}; std::array<bool, 2> effect_seen{};
    for (const ConfigEntry& entry : config.entries) {
        const std::string& key = entry.key;
        const float value = entry.value;
        if (key == "key") p.key_pc = std::clamp(static_cast<int>(value), 0, 11);
        else if (key == "octave") p.octave = std::clamp(static_cast<int>(value), -2, 2);
        else if (key == "preset") p.preset = static_cast<int>(value);
        else if (key == "bpm") p.bpm = std::clamp(static_cast<int>(value), 40, 240);
        else if (key == "mode") p.mode = static_cast<PlayMode>(std::clamp(static_cast<int>(value), 0, 3));
        else if (key == "palette" || key == "defaultpalette") ui.default_palette = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "l1palette" || key == "l2palette") ui.l1_palette = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "selectedpalette") ui.selected_palette = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "direction") ui.selected_direction = static_cast<Direction>(std::clamp(static_cast<int>(value), 0, 8));
        else if (key == "bank" || key == "defaultbank") ui.default_bank = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "r1bank") ui.r1_bank = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "r2bank") ui.r2_bank = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "dpadmode") ui.dpad_mode = static_cast<DpadMode>(std::clamp(static_cast<int>(value), 0, 2));
        else if (key == "voiceleading") p.voice_leading = value != 0.0F;
        else if (key == "perffxenabled") ui.perf_fx_enabled = value != 0.0F;
        else if (key == "language") ui.language = static_cast<Language>(std::clamp(static_cast<int>(value), 0, 1));
        else if (key == "chorddpad") ui.chord_dpad = static_cast<ChordDpadStyle>(std::clamp(static_cast<int>(value), 0, 1));
        else if (key == "uimotion") ui.ui_motion = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "swapleft") ui.swap_left_rear = value != 0.0F;
        else if (key == "swapright") ui.swap_right_rear = value != 0.0F;
        else if (const auto index = parse_indexed_key(key, "param", static_cast<int>(SynthParameter::Count))) {
            params[static_cast<std::size_t>(*index)] = value;
            param_seen[static_cast<std::size_t>(*index)] = true;
        } else if (key == "fx1type") { effects[0].type = static_cast<EffectType>(std::clamp(static_cast<int>(value), 0, SynthEngine::effect_count() - 1)); effect_seen[0] = true; }
        else if (key == "fx1amount") { effects[0].amount = value; effect_seen[0] = true; }
        else if (key == "fx1colour") { effects[0].colour = value; effect_seen[0] = true; }
        else if (key == "fx2type") { effects[1].type = static_cast<EffectType>(std::clamp(static_cast<int>(value), 0, SynthEngine::effect_count() - 1)); effect_seen[1] = true; }
        else if (key == "fx2amount") { effects[1].amount = value; effect_seen[1] = true; }
        else if (key == "fx2colour") { effects[1].colour = value; effect_seen[1] = true; }
        else std::cerr << "config: ignored unknown key on line " << entry.line << ": " << key << '\n';
    }
    p.latch = false;
    if (!ui.perf_fx_enabled && ui.dpad_mode == DpadMode::Fx) ui.dpad_mode = DpadMode::Chord;
    synth.set_preset(p.preset); p.preset = synth.preset_index();
    for (int i = 0; i < static_cast<int>(SynthParameter::Count); ++i) if (param_seen[static_cast<std::size_t>(i)]) synth.set_parameter(static_cast<SynthParameter>(i), params[static_cast<std::size_t>(i)]);
    for (int slot = 0; slot < 2; ++slot) if (effect_seen[static_cast<std::size_t>(slot)]) synth.set_effect(slot, effects[static_cast<std::size_t>(slot)]);
}

'''
sdl = replace_between(sdl, "void load_config(", "void save_config(", new_load, "load config")

new_save = r'''void save_config(const PerformanceState& p, const UiState& ui, const SynthEngine& synth) {
    std::ostringstream out;
    out << "key " << p.key_pc << '\n' << "octave " << p.octave << '\n' << "preset " << synth.preset_index() << '\n'
        << "bpm " << p.bpm << '\n' << "mode " << static_cast<int>(p.mode) << '\n' << "latch 0\n"
        << "defaultpalette " << ui.default_palette << '\n' << "l2palette " << ui.l1_palette << '\n'
        << "selectedpalette " << ui.selected_palette << '\n' << "direction " << static_cast<int>(ui.selected_direction) << '\n'
        << "defaultbank " << ui.default_bank << '\n' << "r1bank " << ui.r1_bank << '\n' << "r2bank " << ui.r2_bank << '\n'
        << "dpadmode " << static_cast<int>(ui.dpad_mode) << '\n'
        << "voiceleading " << (p.voice_leading ? 1 : 0) << '\n'
        << "perffxenabled " << (ui.perf_fx_enabled ? 1 : 0) << '\n'
        << "language " << static_cast<int>(ui.language) << '\n'
        << "chorddpad " << static_cast<int>(ui.chord_dpad) << '\n'
        << "uimotion " << ui.ui_motion << '\n'
        << "swapleft " << (ui.swap_left_rear ? 1 : 0) << '\n' << "swapright " << (ui.swap_right_rear ? 1 : 0) << '\n';
    for (int i = 0; i < static_cast<int>(SynthParameter::Count); ++i) out << "param" << i << ' ' << synth.parameter(static_cast<SynthParameter>(i)) << '\n';
    for (int slot = 0; slot < 2; ++slot) {
        const auto fx = synth.effect(slot); const int n = slot + 1;
        out << "fx" << n << "type " << static_cast<int>(fx.type) << '\n'
            << "fx" << n << "amount " << fx.amount << '\n'
            << "fx" << n << "colour " << fx.colour << '\n';
    }

    std::string error;
    if (!atomic_write_text(config_path(), out.str(), &error)) std::cerr << "config: " << error << '\n';
}

'''
sdl = replace_between(sdl, "void save_config(", "void audio_callback(", new_save, "save config")

sdl = replace_once(
    sdl,
    "void release_face(InputState& in, Face face, std::optional<ChordSpec>& current, SynthEngine& synth) {\n"
    "    in.face[static_cast<std::size_t>(face)] = false;\n"
    "    if (in.active_face && *in.active_face == face) {\n"
    "        in.active_face.reset();\n"
    "        for (int i = 3; i >= 0; --i) if (in.face[static_cast<std::size_t>(i)]) { in.active_face = static_cast<Face>(i); break; }\n"
    "        if (!in.active_face) { current.reset(); synth.release_chord(); }\n"
    "    }\n"
    "}",
    "void press_face(InputState& in, Face face) {\n"
    "    const std::size_t index = static_cast<std::size_t>(face);\n"
    "    if (index >= in.face.size() || in.face[index]) return;\n"
    "    in.face[index] = true;\n"
    "    in.face_order.press(face);\n"
    "    in.active_face = in.face_order.active();\n"
    "}\n"
    "void release_face(InputState& in, Face face, std::optional<ChordSpec>& current, SynthEngine& synth) {\n"
    "    const std::size_t index = static_cast<std::size_t>(face);\n"
    "    if (index < in.face.size()) in.face[index] = false;\n"
    "    in.face_order.release(face);\n"
    "    in.active_face = in.face_order.active();\n"
    "    if (!in.active_face) { current.reset(); synth.release_chord(); }\n"
    "}",
    "face order",
)

new_edit_sound = r'''void edit_sound(UiState& ui, PerformanceState& p, InputState& in, SynthEngine& synth,
                std::optional<ChordSpec>& current, std::vector<int>& previous, int delta) {
    const int layer = in.l1 ? 1 : 0;
    const int row = ui.sound_row[static_cast<std::size_t>(layer)];
    if (layer == 0) {
        if (row == 0) {
            p.preset = (p.preset + delta + SynthEngine::preset_count()) % SynthEngine::preset_count();
            synth.set_preset(p.preset);
            previous.clear();
            if (in.active_face) update_chord(p, ui, in, current, previous, synth);
            toast(ui, synth.preset_info().name);
        } else {
            const auto param = static_cast<SynthParameter>(row - 1);
            synth.set_parameter(param, synth.parameter(param) + static_cast<float>(delta) * 0.02F);
        }
    } else {
        if (row <= 2) { const auto param = static_cast<SynthParameter>(row + static_cast<int>(SynthParameter::Attack)); synth.set_parameter(param, synth.parameter(param) + static_cast<float>(delta) * 0.02F); }
        else if (row == 3) { p.bpm = std::clamp(p.bpm + delta, 40, 240); synth.set_bpm(p.bpm); }
        else { const int mode = (static_cast<int>(p.mode) + delta + 4) % 4; p.mode = static_cast<PlayMode>(mode); synth.set_mode(p.mode); }
    }
}
'''
sdl = replace_between(sdl, "void edit_sound(", "void move_sound_row(", new_edit_sound, "edit sound")
sdl = replace_once(sdl, "edit_sound(ui, p, in, synth, right ? 1 : -1);",
                   "edit_sound(ui, p, in, synth, current, previous, right ? 1 : -1);", "edit sound press call")

new_repeat = r'''void repeat_edit(UiState& ui, PerformanceState& p, InputState& in, SynthEngine& synth,
                 std::optional<ChordSpec>& current, std::vector<int>& previous) {
    if (ui.held_edit == 0) return;
    const Uint32 now = SDL_GetTicks(); const Uint32 age = now - ui.held_since;
    const Uint32 interval = age > 2200U ? 40U : age > 850U ? 90U : 100000U;
    if (now - ui.last_repeat < interval) return;
    if (ui.settings) edit_setting(ui, p, synth, ui.held_edit);
    else if (ui.dpad_mode == DpadMode::Sound) edit_sound(ui, p, in, synth, current, previous, ui.held_edit);
    ui.last_repeat = now;
}

'''
sdl = replace_between(sdl, "void repeat_edit(", "void set_l1(", new_repeat, "repeat edit")
sdl = replace_once(sdl, "repeat_edit(ui, state, input, synth);",
                   "repeat_edit(ui, state, input, synth, current, previous);", "repeat edit call")

fatal_helper = r'''
void show_fatal_error(SDL_Renderer* renderer, Language language, std::string_view english, std::string_view russian) {
    const std::string message = brkchrd_ui::tr(language, english, russian);
    const Uint32 until = SDL_GetTicks() + 3000U;
    while (SDL_GetTicks() < until) {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN) return;
        rect(renderer, 0, 0, kWidth, kHeight, kBg);
        text(renderer, kWidth / 2, 150, brkchrd_ui::tr(language, "FATAL ERROR", "КРИТИЧЕСКАЯ ОШИБКА"), 2, kRed, true, 1);
        text(renderer, kWidth / 2, 190, fit(message, 38), 1, kInk, true, 0);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

'''
sdl = replace_once(sdl, "std::optional<Face> face_button(", fatal_helper + "std::optional<Face> face_button(", "fatal error helper")

reset_helper = r'''
void reset_live_state(UiState& ui, InputState& input, std::optional<ChordSpec>& current,
                      std::vector<int>& previous, SynthEngine& synth) {
    restore_performance_fx(ui, synth);
    input = InputState{};
    current.reset();
    previous.clear();
    ui.held_edit = 0;
    synth.all_notes_off();
}

'''
sdl = replace_once(sdl, "void log_controller(SDL_GameController* controller) {", reset_helper + "void log_controller(SDL_GameController* controller) {", "reset live state")

sdl = replace_once(
    sdl,
    "    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);\n"
    "    if (!renderer) renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);\n"
    "    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);",
    "    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);\n"
    "    if (!renderer) renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);\n"
    "    if (!renderer) { std::cerr << \"renderer: \" << SDL_GetError() << '\\n'; SDL_DestroyWindow(window); SDL_Quit(); return 1; }\n"
    "    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);",
    "renderer check",
)
sdl = replace_once(
    sdl,
    "    SDL_AudioSpec obtained{}; const SDL_AudioDeviceID audio = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);\n"
    "    if (audio != 0) SDL_PauseAudioDevice(audio, 0); else std::cerr << \"audio: \" << SDL_GetError() << '\\n';",
    "    SDL_AudioSpec obtained{}; const SDL_AudioDeviceID audio = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);\n"
    "    if (audio == 0) {\n"
    "        std::cerr << \"audio: \" << SDL_GetError() << '\\n';\n"
    "        show_fatal_error(renderer, ui.language, \"AUDIO INITIALIZATION FAILED\", \"ОШИБКА ИНИЦИАЛИЗАЦИИ АУДИО\");\n"
    "        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit(); return 1;\n"
    "    }\n"
    "    std::cerr << \"audio backend=\" << (SDL_GetCurrentAudioDriver() ? SDL_GetCurrentAudioDriver() : \"unknown\")\n"
    "              << \" rate=\" << obtained.freq << \" frames=\" << obtained.samples\n"
    "              << \" channels=\" << static_cast<int>(obtained.channels) << '\\n';\n"
    "    SDL_PauseAudioDevice(audio, 0);",
    "audio check",
)

sdl = replace_once(
    sdl,
    "            if (event.type == SDL_QUIT) running = false;",
    "            if (event.type == SDL_QUIT) running = false;\n"
    "            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)\n"
    "                reset_live_state(ui, input, current, previous, synth);\n"
    "            if (event.type == SDL_AUDIODEVICEREMOVED && event.adevice.iscapture == 0 && event.adevice.which == audio) {\n"
    "                reset_live_state(ui, input, current, previous, synth);\n"
    "                running = false;\n"
    "            }",
    "focus and audio loss",
)
sdl = replace_once(
    sdl,
    "                SDL_GameControllerClose(controller); controller = nullptr;\n"
    "            }",
    "                reset_live_state(ui, input, current, previous, synth);\n"
    "                SDL_GameControllerClose(controller); controller = nullptr;\n"
    "            }",
    "controller disconnect reset",
)

controller_face_old = "if (down) { input.face[static_cast<std::size_t>(*face)] = true; input.active_face = *face; update_chord(state, ui, input, current, previous, synth); }\n                    else { release_face(input, *face, current, synth); if (input.active_face) update_chord(state, ui, input, current, previous, synth); }"
controller_face_new = "if (down) { press_face(input, *face); update_chord(state, ui, input, current, previous, synth); }\n                    else { release_face(input, *face, current, synth); if (input.active_face) update_chord(state, ui, input, current, previous, synth); }"
sdl = replace_once(sdl, controller_face_old, controller_face_new, "controller face order")

sdl = replace_once(
    sdl,
    "            if (event.type == SDL_CONTROLLERAXISMOTION) {\n"
    "                const bool down = event.caxis.value > 15000;\n"
    "                if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) set_left_physical(ui, state, input, false, down, current, previous, synth);\n"
    "                if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) set_right_physical(ui, state, input, false, down, current, previous, synth);\n"
    "            }",
    "            if (event.type == SDL_CONTROLLERAXISMOTION) {\n"
    "                if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {\n"
    "                    const bool down = trigger_hysteresis(input.left_trigger, event.caxis.value);\n"
    "                    if (down != input.left_trigger) { input.left_trigger = down; set_left_physical(ui, state, input, false, down, current, previous, synth); }\n"
    "                }\n"
    "                if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {\n"
    "                    const bool down = trigger_hysteresis(input.right_trigger, event.caxis.value);\n"
    "                    if (down != input.right_trigger) { input.right_trigger = down; set_right_physical(ui, state, input, false, down, current, previous, synth); }\n"
    "                }\n"
    "            }",
    "trigger hysteresis",
)

keyboard_lambda_old = "                const bool down = event.type == SDL_KEYDOWN; const SDL_Keycode key = event.key.keysym.sym;\n                auto keyboard_face = [&](Face face) {\n                    if (down) { input.face[static_cast<std::size_t>(face)] = true; input.active_face = face; update_chord(state, ui, input, current, previous, synth); }\n                    else { release_face(input, face, current, synth); if (input.active_face) update_chord(state, ui, input, current, previous, synth); }\n                };"
keyboard_lambda_new = "                const bool down = event.type == SDL_KEYDOWN; const SDL_Keycode key = event.key.keysym.sym;\n                const bool repeated = down && event.key.repeat != 0;\n                auto keyboard_face = [&](Face face) {\n                    if (repeated) return;\n                    if (down) { press_face(input, face); update_chord(state, ui, input, current, previous, synth); }\n                    else { release_face(input, face, current, synth); if (input.active_face) update_chord(state, ui, input, current, previous, synth); }\n                };"
sdl = replace_once(sdl, keyboard_lambda_old, keyboard_lambda_new, "keyboard repeat and order")
sdl = replace_once(sdl, "input.active_face.reset(); input.face.fill(false);", "input.face.fill(false); input.face_order.clear(); input.active_face.reset();", "panic face reset")

sdl_path.write_text(sdl)
print("PR1 safety source migration applied")
