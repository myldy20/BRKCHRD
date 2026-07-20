#!/usr/bin/env python3
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return text.replace(old, new, 1)


def sub_once(text: str, pattern: str, replacement: str, label: str) -> str:
    result, count = re.subn(pattern, replacement, text, count=1, flags=re.S)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return result


path = ROOT / "src/sdl_main.cpp"
s = path.read_text(encoding="utf-8")
s = replace_once(s, '#include "brkchrd/synth.hpp"\n', '#include "brkchrd/synth.hpp"\n#include "ui_i18n.hpp"\n', "include i18n")
s = replace_once(s, 'using namespace brkchrd;\n', 'using namespace brkchrd;\nusing brkchrd_ui::ChordDpadStyle;\nusing brkchrd_ui::Language;\n', "type aliases")
s = replace_once(s, 'constexpr const char* kVersion = "0.5.1";', 'constexpr const char* kVersion = "0.5.2";', "version")
s = replace_once(
    s,
    'std::array<std::uint8_t, 7> glyph(char value) {\n    const char ch = static_cast<char>(std::toupper(static_cast<unsigned char>(value)));',
    'std::array<std::uint8_t, 7> glyph(std::uint32_t value) {\n    if (value >= 0x0400U) return brkchrd_ui::cyrillic_glyph(value);\n    const char ch = static_cast<char>(std::toupper(static_cast<unsigned char>(value)));',
    "UTF-8 glyph entry",
)
s = sub_once(
    s,
    r'int text_width\(std::string_view value, int scale = 1, int tracking = 1\) \{.*?\n\}\nvoid text_right',
    '''int text_width(std::string_view value, int scale = 1, int tracking = 1) {
    const int length = brkchrd_ui::codepoint_count(value);
    return length == 0 ? 0 : length * (5 * scale + tracking) - tracking;
}
std::string fit(std::string value, int chars) {
    return brkchrd_ui::fit_utf8(std::move(value), chars);
}
void text(SDL_Renderer* r, int x, int y, std::string_view value, int scale, Colour c,
          bool centered = false, int tracking = 1) {
    if (centered) x -= text_width(value, scale, tracking) / 2;
    colour(r, c);
    int cursor = x;
    std::size_t index = 0;
    while (index < value.size()) {
        const auto rows = glyph(brkchrd_ui::next_codepoint(value, index));
        for (int row = 0; row < 7; ++row) for (int col = 0; col < 5; ++col) {
            if (((rows[static_cast<std::size_t>(row)] >> (4 - col)) & 1U) != 0U) {
                SDL_Rect px{cursor + col * scale, y + row * scale, scale, scale};
                SDL_RenderFillRect(r, &px);
            }
        }
        cursor += 5 * scale + tracking;
    }
}
void text_right''',
    "UTF-8 text renderer",
)
s = replace_once(
    s,
    '    DpadMode dpad_mode = DpadMode::Chord;\n    bool settings = false;',
    '    DpadMode dpad_mode = DpadMode::Chord;\n    Language language = Language::English;\n    ChordDpadStyle chord_dpad = ChordDpadStyle::Toggle;\n    bool settings = false;',
    "UI language state",
)
s = sub_once(
    s,
    r'int display_palette\(const InputState& in, const UiState& ui\).*?std::string percent\(float value\) \{ return .*?; \}',
    '''int display_palette(const InputState& in, const UiState& ui) { return in.l1 ? ui.l1_palette : ui.default_palette; }
int active_bank(const InputState& in, const UiState& ui) { return in.r2 ? ui.r2_bank : in.r1 ? ui.r1_bank : ui.default_bank; }
Direction effective_direction(const InputState& in, const UiState& ui) {
    if (ui.chord_dpad == ChordDpadStyle::Hold) {
        if (ui.settings || ui.dpad_mode != DpadMode::Chord) return Direction::Center;
        return held_direction(in);
    }
    return ui.selected_direction;
}
int effective_palette(const InputState& in, const UiState& ui) {
    if (ui.chord_dpad == ChordDpadStyle::Hold && effective_direction(in, ui) != Direction::Center) return display_palette(in, ui);
    return ui.selected_palette;
}
std::string dpad_mode_name(DpadMode mode, Language language) {
    if (mode == DpadMode::Sound) return brkchrd_ui::tr(language, "SOUND", "ЗВУК");
    if (mode == DpadMode::Fx) return brkchrd_ui::tr(language, "PERF FX", "ПЕРФ FX");
    return brkchrd_ui::tr(language, "CHORD", "АККОРД");
}
std::string percent(float value) { return std::to_string(static_cast<int>(std::round(std::clamp(value, 0.0F, 1.0F) * 100.0F))) + "%"; }''',
    "effective chord colour and labels",
)
s = replace_once(
    s,
    '        else if (key == "perffxenabled") ui.perf_fx_enabled = value != 0.0F;\n        else if (key == "uimotion")',
    '        else if (key == "perffxenabled") ui.perf_fx_enabled = value != 0.0F;\n        else if (key == "language") ui.language = static_cast<Language>(std::clamp(static_cast<int>(value), 0, 1));\n        else if (key == "chorddpad") ui.chord_dpad = static_cast<ChordDpadStyle>(std::clamp(static_cast<int>(value), 0, 1));\n        else if (key == "uimotion")',
    "load localisation settings",
)
s = replace_once(
    s,
    '        << "perffxenabled " << (ui.perf_fx_enabled ? 1 : 0) << \'\\n\'\n        << "uimotion "',
    '        << "perffxenabled " << (ui.perf_fx_enabled ? 1 : 0) << \'\\n\'\n        << "language " << static_cast<int>(ui.language) << \'\\n\'\n        << "chorddpad " << static_cast<int>(ui.chord_dpad) << \'\\n\'\n        << "uimotion "',
    "save localisation settings",
)
s = sub_once(
    s,
    r'void draw_header\(SDL_Renderer\* r, const PerformanceState& p, const UiState& ui, float peak\) \{.*?\n\}',
    '''void draw_header(SDL_Renderer* r, const PerformanceState& p, const UiState& ui, float peak) {
    rect(r, 0, 0, kWidth, 36, kPanel);
    text(r, 12, 8, "BRKCHRD", 2, kInk);
    micro(r, 12, 27, std::string("V") + kVersion, kDim);
    const Colour accent = mode_accent(ui.dpad_mode);
    rect(r, 164, 5, 112, 26, accent);
    text(r, 220, 11, dpad_mode_name(ui.dpad_mode, ui.language), 1, kBg, true, 1);
    micro(r, 292, 9, brkchrd_ui::tr(ui.language, "OCT", "ОКТ"), kDim);
    text(r, 323, 8, (p.octave >= 0 ? "+" : "") + std::to_string(p.octave), 2, kInk, true);
    micro(r, 350, 9, brkchrd_ui::tr(ui.language, "MODE", "РЕЖИМ"), kDim);
    text(r, 405, 8, brkchrd_ui::play_mode(ui.language, p.mode), 1, kInk, true);
    rect(r, 462, 9, 38, 12, kBgLift);
    rect(r, 464, 11, static_cast<int>(34.0F * std::clamp(peak, 0.0F, 1.0F)), 8,
         peak > 0.85F ? kRed : peak > 0.55F ? kOrange : kGreen);
    micro(r, 481, 25, brkchrd_ui::tr(ui.language, "OUT", "ВЫХ"), kDim, true);
}''',
    "localised header",
)
s = sub_once(
    s,
    r'void draw_chord_mode\(SDL_Renderer\* r, const UiState& ui, const InputState& in\) \{.*?\n\}\n\nvoid preset_row',
    '''void draw_chord_mode(SDL_Renderer* r, const UiState& ui, const InputState& in) {
    constexpr int x = 12, y = 46, w = 238, h = 286;
    const int palette = display_palette(in, ui);
    const Direction active_direction = effective_direction(in, ui);
    const int active_palette = effective_palette(in, ui);
    const Colour accent = palette == 2 ? kRed : palette == 1 ? kPurpleBright : kPurple;
    panel_frame(r, x, y, w, h, accent);
    text(r, x + 12, y + 10, brkchrd_ui::palette(ui.language, palette), 2, accent);
    micro(r, x + w - 12, y + 14,
          in.l1 ? brkchrd_ui::tr(ui.language, "L2 HELD", "L2 УДЕРЖ") : brkchrd_ui::tr(ui.language, "L2 ALT", "L2 АЛЬТ"),
          in.l1 ? kWhite : kDim, true);
    micro(r, x + 12, y + 35,
          ui.chord_dpad == ChordDpadStyle::Hold
              ? brkchrd_ui::tr(ui.language, "HOLD DPAD FOR TEMP COLOUR", "УДЕРЖ DPAD ДЛЯ ОКРАСКИ")
              : brkchrd_ui::tr(ui.language, "PRESS DPAD TO SET COLOUR", "НАЖМИ DPAD: ВЫБОР ОКРАСКИ"),
          kDim);

    const int gx = x + 10, gy = y + 58, cw = 68, ch = 56, gap = 6;
    const auto positions = grid_positions(gx, gy, cw, ch, gap);
    for (const auto& item : positions) {
        const Direction d = item.first;
        const int px = item.second.first, py = item.second.second;
        const bool active = active_palette == palette && active_direction == d;
        draw_cell(r, px, py, cw, ch, brkchrd_ui::direction(ui.language, palette, d), active, false, accent);
    }
    const int cx = gx + cw + gap, cy = gy + ch + gap;
    const bool base = active_direction == Direction::Center;
    draw_cell(r, cx, cy, cw, ch, brkchrd_ui::tr(ui.language, "BASE", "ОСНОВА"), base, false, accent);

    const std::string prefix = ui.chord_dpad == ChordDpadStyle::Hold
        ? brkchrd_ui::tr(ui.language, "HELD: ", "УДЕРЖ: ")
        : brkchrd_ui::tr(ui.language, "ACTIVE: ", "АКТИВНО: ");
    const std::string selected = active_direction == Direction::Center
        ? prefix + brkchrd_ui::tr(ui.language, "BASE", "ОСНОВА")
        : prefix + brkchrd_ui::palette(ui.language, active_palette) + " / " + brkchrd_ui::direction(ui.language, active_palette, active_direction);
    micro(r, x + w / 2, y + h - 18, fit(selected, 43), kInk, true);
}

void preset_row''',
    "localised chord mode",
)
s = replace_once(
    s,
    'void preset_row(SDL_Renderer* r, int x, int y, int w, const std::string& value,\n                bool active, Colour accent) {',
    'void preset_row(SDL_Renderer* r, int x, int y, int w, const std::string& value,\n                bool active, Colour accent, Language language) {',
    "preset row language",
)
s = replace_once(s, '    micro(r, x + 9, y + 7, "PRESET", active ? kBg : kDim);', '    micro(r, x + 9, y + 7, brkchrd_ui::tr(language, "PRESET", "ПРЕСЕТ"), active ? kBg : kDim);', "preset label")
s = sub_once(
    s,
    r'void draw_sound_mode\(SDL_Renderer\* r, const PerformanceState& p, const UiState& ui,\n                     const InputState& in, const SynthEngine& synth\) \{.*?\n\}\n\nconst std::array<std::string, 8> kFxBankA',
    '''void draw_sound_mode(SDL_Renderer* r, const PerformanceState& p, const UiState& ui,
                     const InputState& in, const SynthEngine& synth) {
    constexpr int x = 12, y = 46, w = 238, h = 286;
    const int layer = in.l1 ? 1 : 0;
    panel_frame(r, x, y, w, h, kOrange);
    text(r, x + 12, y + 10, brkchrd_ui::tr(ui.language, "SOUND", "ЗВУК"), 2, kOrange);
    micro(r, x + w - 12, y + 14,
          layer == 0 ? brkchrd_ui::tr(ui.language, "L2: ENV", "L2: ОГИБ") : brkchrd_ui::tr(ui.language, "L2 HELD", "L2 УДЕРЖ"),
          layer == 1 ? kWhite : kDim, true);
    micro(r, x + 12, y + 35,
          layer == 0 ? brkchrd_ui::tr(ui.language, "ENGINE / TONE BANK", "ДВИЖОК / ТЕМБР")
                     : brkchrd_ui::tr(ui.language, "ENVELOPE / TIME BANK", "ОГИБАЮЩАЯ / ВРЕМЯ"), kDim);

    const int row = ui.sound_row[static_cast<std::size_t>(layer)];
    if (layer == 0) {
        const auto info = synth.preset_info();
        preset_row(r, x + 10, y + 58, w - 20, info.name, row == 0, kOrange, ui.language);
        const std::array<std::string, 3> names = brkchrd_ui::russian(ui.language)
            ? std::array<std::string, 3>{"ТОН", "ТЕЛО", "ДВИЖЕНИЕ"}
            : std::array<std::string, 3>{"TONE", "BODY", "MOTION"};
        const std::array<std::string, 3> values{percent(synth.parameter(SynthParameter::Tone)),
            percent(synth.parameter(SynthParameter::Body)), percent(synth.parameter(SynthParameter::Motion))};
        const std::array<float, 3> bars{synth.parameter(SynthParameter::Tone), synth.parameter(SynthParameter::Body), synth.parameter(SynthParameter::Motion)};
        for (int i = 0; i < 3; ++i) {
            parameter_row(r, x + 10, y + 119 + i * 45, w - 20,
                          names[static_cast<std::size_t>(i)], values[static_cast<std::size_t>(i)],
                          i + 1 == row, kOrange, bars[static_cast<std::size_t>(i)]);
        }
        micro(r, x + 12, y + h - 19, fit(info.character, 42), kDim);
    } else {
        const std::array<std::string, 5> names = brkchrd_ui::russian(ui.language)
            ? std::array<std::string, 5>{"АТАКА", "ЗАТУХАНИЕ", "ШИРИНА", "BPM", "РЕЖИМ ИГРЫ"}
            : std::array<std::string, 5>{"ATTACK", "RELEASE", "SPREAD", "BPM", "PLAY MODE"};
        const std::array<std::string, 5> values{percent(synth.parameter(SynthParameter::Attack)), percent(synth.parameter(SynthParameter::Release)),
            percent(synth.parameter(SynthParameter::Spread)), std::to_string(p.bpm), brkchrd_ui::play_mode(ui.language, p.mode)};
        const std::array<float, 5> bars{synth.parameter(SynthParameter::Attack), synth.parameter(SynthParameter::Release), synth.parameter(SynthParameter::Spread), -1.0F, -1.0F};
        for (int i = 0; i < 5; ++i) parameter_row(r, x + 10, y + 54 + i * 43, w - 20, names[static_cast<std::size_t>(i)], values[static_cast<std::size_t>(i)], i == row, kOrange, bars[static_cast<std::size_t>(i)]);
    }
}

const std::array<std::string, 8> kFxBankA''',
    "localised sound mode",
)
s = sub_once(
    s,
    r'void draw_fx_mode\(SDL_Renderer\* r, const UiState& ui, const InputState& in\) \{.*?\n\}\n\nvoid chord_button',
    '''void draw_fx_mode(SDL_Renderer* r, const UiState& ui, const InputState& in) {
    constexpr int x = 12, y = 46, w = 238, h = 286;
    static const std::array<std::string, 8> ru_a{"РЕВЕРС", "СТАТТЕР", "ЧОП", "КРАШ", "ДРАЙВ", "ВЭШ", "ГЛУБ. ЭХО", "ФАЗА"};
    static const std::array<std::string, 8> ru_b{"БЕЗДНА", "РЭЙТТРАП", "ГЛИТЧ", "СЛОМ", "ДУМ", "ШИММЕР", "ТУННЕЛЬ", "ЧУЖОЙ"};
    const int layer = in.l1 ? 1 : 0;
    const auto& names = brkchrd_ui::russian(ui.language) ? (layer == 0 ? ru_a : ru_b) : (layer == 0 ? kFxBankA : kFxBankB);
    panel_frame(r, x, y, w, h, kTeal);
    text(r, x + 12, y + 10, brkchrd_ui::tr(ui.language, "PERF FX", "ПЕРФ FX"), 2, kTeal);
    micro(r, x + w - 12, y + 14,
          layer == 0 ? brkchrd_ui::tr(ui.language, "L2: ALT", "L2: АЛЬТ") : brkchrd_ui::tr(ui.language, "L2 HELD", "L2 УДЕРЖ"),
          layer == 1 ? kWhite : kDim, true);
    micro(r, x + 12, y + 35, brkchrd_ui::tr(ui.language, "HOLD DPAD / RELEASE RESTORES", "УДЕРЖ DPAD / ОТПУСТИ"), kDim);
    const int gx = x + 10, gy = y + 58, cw = 68, ch = 56, gap = 6;
    const auto positions = grid_positions(gx, gy, cw, ch, gap);
    for (const auto& item : positions) {
        const int idx = direction_index(item.first);
        const bool live = ui.perf_fx_active && ui.perf_fx_direction == item.first && ui.perf_fx_layer == layer;
        draw_cell(r, item.second.first, item.second.second, cw, ch, names[static_cast<std::size_t>(idx)], false, live, kTeal);
    }
    draw_cell(r, gx + cw + gap, gy + ch + gap, cw, ch, brkchrd_ui::tr(ui.language, "DRY", "СУХО"), false, !ui.perf_fx_active, kTeal);
    micro(r, x + w / 2, y + h - 18,
          layer == 0 ? brkchrd_ui::tr(ui.language, "BANK A: CUT / DIRT / SPACE", "БАНК A: СРЕЗ / ГРЯЗЬ / ЭХО")
                     : brkchrd_ui::tr(ui.language, "BANK B: EXTREME COMBOS", "БАНК B: ЭКСТРЕМАЛЬНЫЙ"), kInk, true);
}

void chord_button''',
    "localised performance FX",
)
s = sub_once(
    s,
    r'void draw_chord_panel\(SDL_Renderer\* r, const PerformanceState& p, const UiState& ui,\n                      const InputState& in, const std::optional<ChordSpec>& current, const SynthEngine& synth\) \{.*?\n\}\n\nconstexpr int kSettingsCount',
    '''void draw_chord_panel(SDL_Renderer* r, const PerformanceState& p, const UiState& ui,
                      const InputState& in, const std::optional<ChordSpec>& current, const SynthEngine& synth) {
    constexpr int x = 262, y = 46, w = 238, h = 286;
    const int bank = active_bank(in, ui);
    const Direction direction = effective_direction(in, ui);
    const int palette = effective_palette(in, ui);
    const Colour accent = bank == 2 ? kBlue : bank == 1 ? kGreen : kPurpleBright;
    panel_frame(r, x, y, w, h, accent);
    text(r, x + 12, y + 10, brkchrd_ui::bank(ui.language, bank), 2, accent);
    const std::string modifier = direction == Direction::Center
        ? brkchrd_ui::tr(ui.language, "BASE", "ОСНОВА")
        : brkchrd_ui::direction(ui.language, palette, direction);
    micro(r, x + w - 12, y + 14, fit(modifier, 17), kInk, true);
    micro(r, x + 12, y + 35, brkchrd_ui::tr(ui.language, "R1 / R2 HOLD CHANGES BANK", "R1 / R2: УДЕРЖ БАНКА"), kDim);

    const float pulse = 0.5F + 0.5F * std::sin(static_cast<float>(SDL_GetTicks()) * 0.012F);
    chord_button(r, x + 83, y + 58, 72, 58, "X", face_label(p.key_pc, Face::Y, bank), in.face[3], kGreen, pulse);
    chord_button(r, x + 12, y + 125, 72, 58, "Y", face_label(p.key_pc, Face::X, bank), in.face[2], kBlue, pulse);
    chord_button(r, x + 154, y + 125, 72, 58, "A", face_label(p.key_pc, Face::B, bank), in.face[1], kRed, pulse);
    chord_button(r, x + 83, y + 192, 72, 58, "B", face_label(p.key_pc, Face::A, bank), in.face[0], kOrange, pulse);

    const std::string chord = current ? current->degree + "  " + current->name : brkchrd_ui::tr(ui.language, "READY", "ГОТОВО");
    text(r, x + w / 2, y + h - 28, fit(chord, 22), current ? 2 : 1, current ? kWhite : kDim, true, 0);
}

constexpr int kSettingsCount''',
    "localised chord panel",
)
s = sub_once(
    s,
    r'constexpr int kSettingsCount = 18;.*?\n\}\n\nvoid draw_settings',
    '''constexpr int kSettingsCount = 20;
std::string settings_name(int row, const UiState& ui) {
    static const std::array<std::string, kSettingsCount> en{
        "BASE COLOUR", "L2 COLOUR", "BASE BANK", "R1 BANK", "R2 BANK",
        "FX1 TYPE", "FX1 AMOUNT", "FX1 COLOUR", "FX2 TYPE", "FX2 AMOUNT", "FX2 COLOUR",
        "KEY", "OCTAVE", "VOICE LEAD", "PERF FX", "UI MOTION", "SWAP L1/L2", "SWAP R1/R2",
        "LANGUAGE / ЯЗЫК", "CHORD DPAD"};
    static const std::array<std::string, kSettingsCount> ru{
        "ОСН. ОКРАСКА", "L2 ОКРАСКА", "ОСН. БАНК", "R1 БАНК", "R2 БАНК",
        "FX1 ТИП", "FX1 УРОВЕНЬ", "FX1 ОКРАСКА", "FX2 ТИП", "FX2 УРОВЕНЬ", "FX2 ОКРАСКА",
        "ТОНАЛЬНОСТЬ", "ОКТАВА", "ВЕДЕНИЕ", "ПЕРФ FX", "АНИМАЦИЯ", "СМЕНА L1/L2", "СМЕНА R1/R2",
        "LANGUAGE / ЯЗЫК", "DPAD АККОРД"};
    return (brkchrd_ui::russian(ui.language) ? ru : en)[static_cast<std::size_t>(row)];
}
std::string settings_value(int row, const PerformanceState& p, const UiState& ui, const SynthEngine& synth) {
    const auto fx1 = synth.effect(0); const auto fx2 = synth.effect(1);
    switch (row) {
    case 0: return brkchrd_ui::palette(ui.language, ui.default_palette); case 1: return brkchrd_ui::palette(ui.language, ui.l1_palette);
    case 2: return brkchrd_ui::bank(ui.language, ui.default_bank); case 3: return brkchrd_ui::bank(ui.language, ui.r1_bank); case 4: return brkchrd_ui::bank(ui.language, ui.r2_bank);
    case 5: return brkchrd_ui::effect(ui.language, fx1.type); case 6: return percent(fx1.amount); case 7: return percent(fx1.colour);
    case 8: return brkchrd_ui::effect(ui.language, fx2.type); case 9: return percent(fx2.amount); case 10: return percent(fx2.colour);
    case 11: return note_name(p.key_pc); case 12: return (p.octave >= 0 ? "+" : "") + std::to_string(p.octave);
    case 13: return brkchrd_ui::on_off(ui.language, p.voice_leading); case 14: return brkchrd_ui::on_off(ui.language, ui.perf_fx_enabled);
    case 15: return brkchrd_ui::ui_motion(ui.language, ui.ui_motion); case 16: return brkchrd_ui::on_off(ui.language, ui.swap_left_rear); case 17: return brkchrd_ui::on_off(ui.language, ui.swap_right_rear);
    case 18: return ui.language == Language::Russian ? "RU" : "EN";
    case 19: return brkchrd_ui::chord_dpad(ui.language, ui.chord_dpad);
    default: return "";
    }
}

void draw_settings''',
    "settings rows",
)
s = sub_once(
    s,
    r'void draw_settings\(SDL_Renderer\* r, const PerformanceState& p, const UiState& ui, const SynthEngine& synth\) \{.*?\n\}\n\nvoid draw_footer',
    '''void draw_settings(SDL_Renderer* r, const PerformanceState& p, const UiState& ui, const SynthEngine& synth) {
    rect(r, 0, 0, kWidth, kHeight, alpha(kBg, 248));
    panel_frame(r, 12, 12, 488, 360, kBlue);
    text(r, 28, 28, brkchrd_ui::tr(ui.language, "SETTINGS", "НАСТРОЙКИ"), 3, kBlue);
    micro(r, 486, 38, brkchrd_ui::tr(ui.language, "SELECT: CLOSE", "SELECT: ЗАКРЫТЬ"), kDim, true);
    micro(r, 28, 58, brkchrd_ui::tr(ui.language, "UP/DOWN SELECT  LEFT/RIGHT CHANGE", "ВВЕРХ/ВНИЗ: ВЫБОР  ВЛЕВО/ВПРАВО: ИЗМ."), kDim);
    const int visible = 7;
    const int start = std::clamp(ui.settings_scroll, 0, kSettingsCount - visible);
    for (int i = 0; i < visible; ++i) {
        const int row = start + i; const bool active = row == ui.settings_row;
        rect(r, 28, 78 + i * 38, 456, 31, active ? kBlue : kPanel2);
        outline(r, 28, 78 + i * 38, 456, 31, active ? kWhite : alpha(kBlue, 100));
        text(r, 40, 88 + i * 38, fit(settings_name(row, ui), 18), 1, active ? kBg : kInk, false, 0);
        text_right(r, 466, 88 + i * 38, fit(settings_value(row, p, ui, synth), 22), 1, active ? kBg : kDim, 0);
    }
    micro(r, 28, 352, brkchrd_ui::tr(ui.language, "START+SELECT: SAVE AND EXIT", "START+SELECT: СОХР. И ВЫХОД"), kDim);
    micro(r, 484, 352, std::to_string(ui.settings_row + 1) + "/" + std::to_string(kSettingsCount), kDim, true);
}

void draw_footer''',
    "localised settings screen",
)
s = sub_once(
    s,
    r'void draw_footer\(SDL_Renderer\* r, const UiState& ui, const InputState& in\) \{.*?\n\}\n\nvoid draw_ui',
    '''void draw_footer(SDL_Renderer* r, const UiState& ui, const InputState& in) {
    rect(r, 0, 340, kWidth, 44, kPanel);
    if (ui.dpad_mode == DpadMode::Chord) {
        micro(r, 14, 350, brkchrd_ui::tr(ui.language, "L2 HOLD ALT COLOUR", "L2: АЛЬТ ОКРАСКА"), kInk);
        micro(r, 14, 366, ui.perf_fx_enabled
            ? brkchrd_ui::tr(ui.language, "L1: SOUND / PERF FX", "L1: ЗВУК / ПЕРФ FX")
            : brkchrd_ui::tr(ui.language, "L1: SOUND / CHORD", "L1: ЗВУК / АККОРД"), kDim);
    } else if (ui.dpad_mode == DpadMode::Sound) {
        micro(r, 14, 350, brkchrd_ui::tr(ui.language, "DPAD EDITS SOUND", "DPAD: ПРАВКА ЗВУКА"), kInk);
        micro(r, 14, 366, brkchrd_ui::tr(ui.language, "L2 HOLD SECOND PARAM BANK", "L2: ВТОРОЙ НАБОР"), kDim);
    } else {
        micro(r, 14, 350, brkchrd_ui::tr(ui.language, "HOLD DPAD FOR PERFORMANCE FX", "УДЕРЖ DPAD: ПЕРФ FX"), kInk);
        micro(r, 14, 366, brkchrd_ui::tr(ui.language, "L2 HOLD SECOND FX BANK", "L2: ВТОРОЙ FX БАНК"), kDim);
    }
    micro(r, 286, 350, brkchrd_ui::tr(ui.language, "R1/R2 HOLD CHORD BANKS", "R1/R2: БАНКИ АККОРДОВ"), kInk);
    micro(r, 286, 366, brkchrd_ui::tr(ui.language, "SELECT SETTINGS  START MODE", "SELECT: НАСТР.  START: РЕЖИМ"), kDim);
    if (in.l1) rect(r, 238, 347, 8, 8, kWhite);
}

void draw_ui''',
    "localised footer",
)
s = replace_once(
    s,
    '    p.colour_palette = ui.selected_palette;\n    const auto chord = make_chord(p.key_pc, p.octave, *in.active_face, bank,\n                                  ui.selected_palette, ui.selected_direction);',
    '    const int palette = effective_palette(in, ui);\n    const Direction direction = effective_direction(in, ui);\n    p.colour_palette = palette;\n    const auto chord = make_chord(p.key_pc, p.octave, *in.active_face, bank, palette, direction);',
    "effective colour in chord engine",
)
s = replace_once(s, '    toast(ui, "OCTAVE " + std::string(p.octave >= 0 ? "+" : "") + std::to_string(p.octave));', '    toast(ui, brkchrd_ui::tr(ui.language, "OCTAVE ", "ОКТАВА ") + std::string(p.octave >= 0 ? "+" : "") + std::to_string(p.octave));', "octave toast")
s = replace_once(s, '    toast(ui, "DPAD: " + dpad_mode_name(ui.dpad_mode));', '    toast(ui, "DPAD: " + dpad_mode_name(ui.dpad_mode, ui.language));', "mode toast")
s = replace_once(
    s,
    '    else if (row == 16) ui.swap_left_rear = !ui.swap_left_rear;\n    else if (row == 17) ui.swap_right_rear = !ui.swap_right_rear;',
    '    else if (row == 16) ui.swap_left_rear = !ui.swap_left_rear;\n    else if (row == 17) ui.swap_right_rear = !ui.swap_right_rear;\n    else if (row == 18) ui.language = ui.language == Language::English ? Language::Russian : Language::English;\n    else if (row == 19) ui.chord_dpad = ui.chord_dpad == ChordDpadStyle::Toggle ? ChordDpadStyle::Hold : ChordDpadStyle::Toggle;',
    "edit language settings",
)
s = replace_once(s, '    } else if (ui.dpad_mode == DpadMode::Chord) {', '    } else if (ui.dpad_mode == DpadMode::Chord && ui.chord_dpad == ChordDpadStyle::Toggle) {', "toggle chord handler")
s = replace_once(
    s,
    '            toast(ui, ui.selected_direction == Direction::Center ? "BASE" : palette_name(ui.selected_palette) + " / " + direction_label(ui.selected_palette, ui.selected_direction));',
    '            toast(ui, ui.selected_direction == Direction::Center\n                ? brkchrd_ui::tr(ui.language, "BASE", "ОСНОВА")\n                : brkchrd_ui::palette(ui.language, ui.selected_palette) + " / " + brkchrd_ui::direction(ui.language, ui.selected_palette, ui.selected_direction));',
    "toggle colour toast",
)
s = replace_once(
    s,
    'void repeat_edit(UiState& ui, PerformanceState& p, const InputState& in, SynthEngine& synth) {',
    '''void update_chord_hold(UiState& ui, PerformanceState& p, InputState& in, SynthEngine& synth,
                       std::optional<ChordSpec>& current, std::vector<int>& previous) {
    if (ui.settings || ui.dpad_mode != DpadMode::Chord || ui.chord_dpad != ChordDpadStyle::Hold) return;
    const Direction direction = effective_direction(in, ui);
    const int palette = effective_palette(in, ui);
    toast(ui, direction == Direction::Center
        ? brkchrd_ui::tr(ui.language, "BASE", "ОСНОВА")
        : brkchrd_ui::palette(ui.language, palette) + " / " + brkchrd_ui::direction(ui.language, palette, direction));
    if (in.active_face) update_chord(p, ui, in, current, previous, synth);
}

void repeat_edit(UiState& ui, PerformanceState& p, const InputState& in, SynthEngine& synth) {''',
    "hold chord refresh",
)
controller_old = '''                    set_dpad(input, button, down);
                    if (ui.dpad_mode == DpadMode::Fx && !ui.settings) update_performance_fx(ui, input, synth);
                    else if (down) handle_dpad_press(ui, state, input, synth, button, current, previous);
                    if (!down && (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT || button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) ui.held_edit = 0;'''
controller_new = '''                    set_dpad(input, button, down);
                    if (ui.dpad_mode == DpadMode::Fx && !ui.settings) update_performance_fx(ui, input, synth);
                    else if (ui.dpad_mode == DpadMode::Chord && ui.chord_dpad == ChordDpadStyle::Hold && !ui.settings)
                        update_chord_hold(ui, state, input, synth, current, previous);
                    else if (down) handle_dpad_press(ui, state, input, synth, button, current, previous);
                    if (!down && (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT || button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) ui.held_edit = 0;'''
s = replace_once(s, controller_old, controller_new, "controller hold mode")
keyboard_old = '''                    set_dpad(input, button, down);
                    if (ui.dpad_mode == DpadMode::Fx && !ui.settings) update_performance_fx(ui, input, synth);
                    else if (down) handle_dpad_press(ui, state, input, synth, button, current, previous);
                    if (!down && (key == SDLK_LEFT || key == SDLK_RIGHT)) ui.held_edit = 0;'''
keyboard_new = '''                    set_dpad(input, button, down);
                    if (ui.dpad_mode == DpadMode::Fx && !ui.settings) update_performance_fx(ui, input, synth);
                    else if (ui.dpad_mode == DpadMode::Chord && ui.chord_dpad == ChordDpadStyle::Hold && !ui.settings)
                        update_chord_hold(ui, state, input, synth, current, previous);
                    else if (down) handle_dpad_press(ui, state, input, synth, button, current, previous);
                    if (!down && (key == SDLK_LEFT || key == SDLK_RIGHT)) ui.held_edit = 0;'''
s = replace_once(s, keyboard_old, keyboard_new, "keyboard hold mode")
s = replace_once(s, '                else if (key == SDLK_w) set_l1(ui, input, down, synth);', '                else if (key == SDLK_w) { set_l1(ui, input, down, synth); if (input.active_face) update_chord(state, ui, input, current, previous, synth); }', "keyboard alt palette")
s = replace_once(
    s,
    '                        if (!input.start_long && !input.select) { state.mode = static_cast<PlayMode>((static_cast<int>(state.mode) + 1) % 4); synth.set_mode(state.mode); toast(ui, mode_name(state.mode)); }',
    '                        if (!input.start_long && !input.select) { state.mode = static_cast<PlayMode>((static_cast<int>(state.mode) + 1) % 4); synth.set_mode(state.mode); toast(ui, brkchrd_ui::play_mode(ui.language, state.mode)); }',
    "play mode toast",
)
s = replace_once(
    s,
    '                        if (!input.start) { restore_performance_fx(ui, synth); ui.settings = !ui.settings; ui.held_edit = 0; toast(ui, ui.settings ? "SETTINGS" : "BACK TO PLAY"); if (!ui.settings) save_config(state,  ui, synth); }',
    '                        if (!input.start) { restore_performance_fx(ui, synth); ui.settings = !ui.settings; ui.held_edit = 0; if (input.active_face) update_chord(state, ui, input, current, previous, synth); toast(ui, ui.settings ? brkchrd_ui::tr(ui.language, "SETTINGS", "НАСТРОЙКИ") : brkchrd_ui::tr(ui.language, "BACK TO PLAY", "ВОЗВРАТ К ИГРЕ")); if (!ui.settings) save_config(state, ui, synth); }',
    "settings toggle toast",
)
s = replace_once(
    s,
    '                    restore_performance_fx(ui, synth); ui.settings = !ui.settings; if (!ui.settings) save_config(state, ui, synth);',
    '                    restore_performance_fx(ui, synth); ui.settings = !ui.settings; if (input.active_face) update_chord(state, ui, input, current, previous, synth); if (!ui.settings) save_config(state, ui, synth);',
    "keyboard settings chord refresh",
)
s = replace_once(s, '    toast(ui, "BRKCHRD 0.5  SMART VOICING", 1500U);', '    toast(ui, brkchrd_ui::tr(ui.language, "BRKCHRD 0.5.2  LIVE CHORDS", "BRKCHRD 0.5.2  ЖИВЫЕ АККОРДЫ"), 1500U);', "startup toast")
s = replace_once(s, 'toast(ui, "ALL NOTES OFF", 1200U);', 'toast(ui, brkchrd_ui::tr(ui.language, "ALL NOTES OFF", "ВСЕ НОТЫ ВЫКЛ"), 1200U);', "panic toast")
path.write_text(s, encoding="utf-8")

simple_replacements = {
    "CMakeLists.txt": [("project(brkchrd VERSION 0.5.1", "project(brkchrd VERSION 0.5.2")],
    "tools/render_demo.cpp": [("brkchrd-v051-demo.wav", "brkchrd-v052-demo.wav")],
    "tools/package_portmaster.sh": [("brkchrd-v0.5.1-portmaster.zip", "brkchrd-v0.5.2-portmaster.zip")],
    "tools/package_nextui.sh": [("brkchrd-v0.5.1-nextui-tg5040.zip", "brkchrd-v0.5.2-nextui-tg5040.zip")],
    "packaging/portmaster/brkchrd/README.md": [("BRKCHRD 0.5.1", "BRKCHRD 0.5.2")],
    "packaging/nextui/tg5040/BRKCHRD.pak/README.md": [("BRKCHRD 0.5.1", "BRKCHRD 0.5.2")],
}
for relative, replacements in simple_replacements.items():
    target = ROOT / relative
    value = target.read_text(encoding="utf-8")
    for old, new in replacements:
        if old not in value:
            raise RuntimeError(f"{relative}: missing {old}")
        value = value.replace(old, new)
    target.write_text(value, encoding="utf-8")

# Replace the temporary migration workflow with the permanent CI definition.
(ROOT / ".github/workflows/build.yml").write_text((ROOT / "tools/build-v052.yml").read_text(encoding="utf-8"), encoding="utf-8")
(ROOT / "tools/build-v052.yml").unlink()
Path(__file__).unlink()
