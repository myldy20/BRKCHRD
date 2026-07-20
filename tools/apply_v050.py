from pathlib import Path

root = Path(__file__).resolve().parents[1]
path = root / 'src/sdl_main.cpp'
s = path.read_text()


def replace(old: str, new: str, count: int = 1) -> None:
    global s
    found = s.count(old)
    if found < count:
        raise RuntimeError(f'missing replacement ({found} < {count}): {old[:120]!r}')
    s = s.replace(old, new, count)

replace('constexpr const char* kVersion = "0.4.0";', 'constexpr const char* kVersion = "0.5.0";')
replace("    case '%': return {0x19,0x19,0x02,0x04,0x08,0x13,0x13};", "    case '@': return {0x0E,0x11,0x17,0x15,0x17,0x10,0x0E};\n    case '%': return {0x19,0x19,0x02,0x04,0x08,0x13,0x13};")

replace('''    int ui_motion = 2;
    bool swap_left_rear = false;
    bool swap_right_rear = false;
''', '''    int ui_motion = 2;
    bool perf_fx_enabled = true;
    bool swap_left_rear = false;
    bool swap_right_rear = false;
''')

replace('''        else if (key == "l1palette") ui.l1_palette = std::clamp(static_cast<int>(value), 0, 2);
''', '''        else if (key == "l1palette" || key == "l2palette") ui.l1_palette = std::clamp(static_cast<int>(value), 0, 2);
''')
replace('''        else if (key == "uimotion") ui.ui_motion = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "swapleft") ui.swap_left_rear = value != 0.0F;
''', '''        else if (key == "voiceleading") p.voice_leading = value != 0.0F;
        else if (key == "perffxenabled") ui.perf_fx_enabled = value != 0.0F;
        else if (key == "uimotion") ui.ui_motion = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "swapleft") ui.swap_left_rear = value != 0.0F;
''')
replace('''    p.latch = false;
    synth.set_preset(p.preset); p.preset = synth.preset_index();
''', '''    p.latch = false;
    if (!ui.perf_fx_enabled && ui.dpad_mode == DpadMode::Fx) ui.dpad_mode = DpadMode::Chord;
    synth.set_preset(p.preset); p.preset = synth.preset_index();
''')
replace(r'''        << "defaultpalette " << ui.default_palette << '\n' << "l1palette " << ui.l1_palette << '\n'
''', r'''        << "defaultpalette " << ui.default_palette << '\n' << "l2palette " << ui.l1_palette << '\n'
''')
replace(r'''        << "dpadmode " << static_cast<int>(ui.dpad_mode) << '\n' << "uimotion " << ui.ui_motion << '\n'
''', r'''        << "dpadmode " << static_cast<int>(ui.dpad_mode) << '\n'
        << "voiceleading " << (p.voice_leading ? 1 : 0) << '\n'
        << "perffxenabled " << (ui.perf_fx_enabled ? 1 : 0) << '\n'
        << "uimotion " << ui.ui_motion << '\n'
''')

replace('''    bar(r, x + 12, y + h - 9, w - 24, synth.output_peak(), accent);
''', '')

start = s.index('constexpr int kSettingsCount = 16;')
end = s.index('\nvoid draw_settings(', start)
settings_block = r'''constexpr int kSettingsCount = 18;
std::string settings_name(int row) {
    static const std::array<std::string, kSettingsCount> names{
        "BASE COLOUR", "L2 COLOUR", "BASE BANK", "R1 BANK", "R2 BANK",
        "FX1 TYPE", "FX1 AMOUNT", "FX1 COLOUR", "FX2 TYPE", "FX2 AMOUNT", "FX2 COLOUR",
        "KEY", "OCTAVE", "VOICE LEAD", "PERF FX", "UI MOTION", "SWAP L1/L2", "SWAP R1/R2"};
    return names[static_cast<std::size_t>(row)];
}
std::string settings_value(int row, const PerformanceState& p, const UiState& ui, const SynthEngine& synth) {
    const auto fx1 = synth.effect(0); const auto fx2 = synth.effect(1);
    switch (row) {
    case 0: return palette_name(ui.default_palette); case 1: return palette_name(ui.l1_palette);
    case 2: return bank_name(ui.default_bank); case 3: return bank_name(ui.r1_bank); case 4: return bank_name(ui.r2_bank);
    case 5: return SynthEngine::effect_name(fx1.type); case 6: return percent(fx1.amount); case 7: return percent(fx1.colour);
    case 8: return SynthEngine::effect_name(fx2.type); case 9: return percent(fx2.amount); case 10: return percent(fx2.colour);
    case 11: return note_name(p.key_pc); case 12: return (p.octave >= 0 ? "+" : "") + std::to_string(p.octave);
    case 13: return on_off(p.voice_leading); case 14: return on_off(ui.perf_fx_enabled);
    case 15: return ui_motion_name(ui.ui_motion); case 16: return on_off(ui.swap_left_rear); case 17: return on_off(ui.swap_right_rear);
    default: return "";
    }
}
'''
s = s[:start] + settings_block + s[end:]

start = s.index('void draw_footer(')
end = s.index('\nvoid draw_ui(', start)
footer_block = r'''void draw_footer(SDL_Renderer* r, const UiState& ui, const InputState& in) {
    rect(r, 0, 340, kWidth, 44, kPanel);
    if (ui.dpad_mode == DpadMode::Chord) {
        micro(r, 14, 350, "L2 HOLD ALT COLOUR", kInk);
        micro(r, 14, 366, ui.perf_fx_enabled ? "L1: SOUND / PERF FX" : "L1: SOUND / CHORD", kDim);
    } else if (ui.dpad_mode == DpadMode::Sound) {
        micro(r, 14, 350, "DPAD EDITS SOUND", kInk);
        micro(r, 14, 366, "L2 HOLD SECOND PARAM BANK", kDim);
    } else {
        micro(r, 14, 350, "HOLD DPAD FOR PERFORMANCE FX", kInk);
        micro(r, 14, 366, "L2 HOLD SECOND FX BANK", kDim);
    }
    micro(r, 286, 350, "R1/R2 HOLD CHORD BANKS", kInk);
    micro(r, 286, 366, "SELECT SETTINGS  START MODE", kDim);
    if (in.l1) rect(r, 238, 347, 8, 8, kWhite);
}
'''
s = s[:start] + footer_block + s[end:]

replace('''    const auto notes = voice_lead(chord, previous);
''', '''    const auto notes = voice_chord(chord, previous, p.voice_leading, synth.voicing_profile());
''')
replace('''    if (ui.settings || ui.dpad_mode != DpadMode::Fx) { restore_performance_fx(ui, synth); return; }
''', '''    if (ui.settings || !ui.perf_fx_enabled || ui.dpad_mode != DpadMode::Fx) { restore_performance_fx(ui, synth); return; }
''')

start = s.index('void cycle_dpad_mode(')
end = s.index('\nvoid edit_sound(', start)
cycle_block = r'''void cycle_dpad_mode(UiState& ui, InputState& in, SynthEngine& synth) {
    restore_performance_fx(ui, synth);
    if (ui.dpad_mode == DpadMode::Chord) ui.dpad_mode = DpadMode::Sound;
    else if (ui.dpad_mode == DpadMode::Sound) ui.dpad_mode = ui.perf_fx_enabled ? DpadMode::Fx : DpadMode::Chord;
    else ui.dpad_mode = DpadMode::Chord;
    ui.held_edit = 0;
    toast(ui, "DPAD: " + dpad_mode_name(ui.dpad_mode));
    if (ui.dpad_mode == DpadMode::Fx) update_performance_fx(ui, in, synth);
}
'''
s = s[:start] + cycle_block + s[end:]

start = s.index('void edit_setting(')
end = s.index('\nvoid move_settings_row(', start)
edit_settings = r'''void edit_setting(UiState& ui, PerformanceState& p, SynthEngine& synth, int delta) {
    const int row = ui.settings_row;
    auto cycle3 = [delta](int value) { return (value + delta + 3) % 3; };
    if (row == 0) ui.default_palette = cycle3(ui.default_palette);
    else if (row == 1) ui.l1_palette = cycle3(ui.l1_palette);
    else if (row == 2) ui.default_bank = cycle3(ui.default_bank);
    else if (row == 3) ui.r1_bank = cycle3(ui.r1_bank);
    else if (row == 4) ui.r2_bank = cycle3(ui.r2_bank);
    else if (row >= 5 && row <= 10) {
        const int slot = row >= 8 ? 1 : 0; const int local = slot == 0 ? row - 5 : row - 8;
        auto settings = synth.effect(slot);
        if (local == 0) settings.type = static_cast<EffectType>((static_cast<int>(settings.type) + delta + SynthEngine::effect_count()) % SynthEngine::effect_count());
        else if (local == 1) settings.amount += static_cast<float>(delta) * 0.05F;
        else settings.colour += static_cast<float>(delta) * 0.05F;
        synth.set_effect(slot, settings);
    } else if (row == 11) p.key_pc = (p.key_pc + delta + 12) % 12;
    else if (row == 12) p.octave = std::clamp(p.octave + delta, -2, 2);
    else if (row == 13) p.voice_leading = !p.voice_leading;
    else if (row == 14) {
        ui.perf_fx_enabled = !ui.perf_fx_enabled;
        if (!ui.perf_fx_enabled) {
            restore_performance_fx(ui, synth);
            if (ui.dpad_mode == DpadMode::Fx) ui.dpad_mode = DpadMode::Chord;
        }
    }
    else if (row == 15) ui.ui_motion = std::clamp(ui.ui_motion + delta, 0, 2);
    else if (row == 16) ui.swap_left_rear = !ui.swap_left_rear;
    else if (row == 17) ui.swap_right_rear = !ui.swap_right_rear;
}
'''
s = s[:start] + edit_settings + s[end:]

replace('''    const bool logical_l1 = first != ui.swap_left_rear;
    if (logical_l1) set_l1(ui, in, down, synth); else set_l2(ui, in, down, synth);
''', '''    const bool physical_l1 = first != ui.swap_left_rear;
    if (physical_l1) set_l2(ui, in, down, synth); else set_l1(ui, in, down, synth);
''')
replace(r'''    std::cerr << "roles: shoulders=front octave, leftstick=L1, lefttrigger=L2, rightstick=R1, righttrigger=R2\n";
''', r'''    std::cerr << "roles: shoulders=front octave, physical L1=mode, physical L2=alt layer, R1/R2=chord banks\n";
''')
replace('''                else if (key == SDLK_q) set_l1(ui, input, down, synth);
                else if (key == SDLK_w && down && event.key.repeat == 0) set_l2(ui, input, true, synth);
                else if (key == SDLK_w && !down) set_l2(ui, input, false, synth);
''', '''                else if (key == SDLK_q && down && event.key.repeat == 0) set_l2(ui, input, true, synth);
                else if (key == SDLK_q && !down) set_l2(ui, input, false, synth);
                else if (key == SDLK_w) set_l1(ui, input, down, synth);
''')

for old, new in {
    '"L1 HELD"': '"L2 HELD"',
    '"L1 ALT"': '"L2 ALT"',
    '"L1: ENV"': '"L2: ENV"',
    '"L1: ALT"': '"L2: ALT"',
    '"L1 HOLD SECOND PARAM BANK"': '"L2 HOLD SECOND PARAM BANK"',
    '"L1 HOLD SECOND FX BANK"': '"L2 HOLD SECOND FX BANK"',
}.items():
    s = s.replace(old, new)

splash = r'''
bool show_splash(SDL_Renderer* r) {
    if (std::getenv("BRKCHRD_SKIP_SPLASH") != nullptr) return true;
    const Uint32 until = SDL_GetTicks() + 2000U;
    while (SDL_GetTicks() < until) {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) if (event.type == SDL_QUIT) return false;
        rect(r, 0, 0, kWidth, kHeight, kBg);
        for (int y = 0; y < kHeight; y += 24) line(r, 0, y, kWidth, y, alpha(kPurple, 18));
        text(r, kWidth / 2, 116, "BRKCHRD", 4, kInk, true, 2);
        line(r, 148, 164, 364, 164, kPurpleBright);
        text(r, kWidth / 2, 194, "developed by myldy design", 1, kDim, true, 1);
        text(r, kWidth / 2, 220, "@myldy20", 2, kPurpleBright, true, 1);
        SDL_RenderPresent(r);
        SDL_Delay(16);
    }
    return true;
}
'''
insert_at = s.index('\nstd::optional<Face> face_button(')
s = s[:insert_at] + '\n' + splash + s[insert_at:]

replace('''    SDL_RenderSetLogicalSize(renderer, kWidth, kHeight);

    SynthEngine synth(kRate); PerformanceState state; UiState ui;
''', '''    SDL_RenderSetLogicalSize(renderer, kWidth, kHeight);
    if (!show_splash(renderer)) {
        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit(); return 0;
    }

    SynthEngine synth(kRate); PerformanceState state; UiState ui;
''')
replace('''    toast(ui, "BRKCHRD 0.4  PERFORMANCE WORKFLOW", 1500U);
''', '''    toast(ui, "BRKCHRD 0.5  SMART VOICING", 1500U);
''')

path.write_text(s)

cmake = root / 'CMakeLists.txt'
cmake.write_text(cmake.read_text().replace('project(brkchrd VERSION 0.4.0', 'project(brkchrd VERSION 0.5.0'))

package = root / 'tools/package_portmaster.sh'
package.write_text(package.read_text().replace('brkchrd-v0.4.0-portmaster.zip', 'brkchrd-v0.5.0-portmaster.zip'))

workflow = root / '.github/workflows/build.yml'
w = workflow.read_text()
w = w.replace('v040', 'v050').replace('v0.4.0', 'v0.5.0')
w = w.replace('SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 3s', 'BRKCHRD_SKIP_SPLASH=1 SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 3s')
workflow.write_text(w)
