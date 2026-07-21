from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return text.replace(old, new, 1)


# Public synth API -----------------------------------------------------------
path = ROOT / "include/brkchrd/synth.hpp"
text = path.read_text()
text = replace_once(
    text,
    "    void play_chord(const std::vector<int>& notes);\n    void release_chord();",
    "    void play_chord(const std::vector<int>& notes);\n"
    "    void change_chord(const std::vector<int>& notes);\n"
    "    void release_chord();",
    "synth header legato API",
)
path.write_text(text)


# Voice lifecycle and rapid-playing safety ----------------------------------
path = ROOT / "src/synth.cpp"
text = path.read_text()
text = replace_once(
    text,
    "    bool active = false;\n    bool gate = false;\n    int note = 60;",
    "    bool active = false;\n    bool gate = false;\n    bool fast_release = false;\n    int note = 60;",
    "voice fast release flag",
)

old_voice_block = '''    void release_all() { for (auto& voice : voices) voice.gate = false; }
    void stop_note(int note) { for (auto& voice : voices) if (voice.active && voice.note == note) voice.gate = false; }

    void start_note(int note) {
        Voice* selected = nullptr;
        for (auto& voice : voices) if (!voice.active) { selected = &voice; break; }
        if (selected == nullptr) {
            selected = &*std::min_element(voices.begin(), voices.end(), [](const Voice& a, const Voice& b) {
                if (a.env != b.env) return a.env < b.env;
                return a.age < b.age;
            });
        }
        selected->active = true;
        selected->gate = true;
        selected->note = note;
        selected->p1 = 0.0;
        selected->p2 = 0.17;
        selected->p3 = 0.41;
        selected->env = std::min(selected->env, 0.05F);
        selected->life = 0.0F;
        selected->low = 0.0F;
        selected->band = 0.0F;
        selected->pan = std::clamp(static_cast<float>((note % 12) - 6) / 9.0F, -0.68F, 0.68F);
        selected->noise = static_cast<std::uint32_t>(note * 747796405U + static_cast<unsigned>(voice_age));
        selected->age = ++voice_age;
    }

    void schedule_chord(const std::vector<int>& notes, bool strum) {
        release_all();
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
        else release_all();
    }
'''
new_voice_block = '''    void release_all(bool fast = false) {
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
'''
text = replace_once(text, old_voice_block, new_voice_block, "voice lifecycle block")
text = replace_once(text, "            if (arp_note >= 0) stop_note(arp_note);", "            if (arp_note >= 0) stop_note(arp_note, true);", "arp fast release")
text = replace_once(text, "            release_all();\n            for (int note : held_notes) start_note(note);", "            release_all(true);\n            for (int note : held_notes) start_note(note);", "pulse fast release")
text = replace_once(
    text,
    "        const float release_step = 1.0F / std::max(1.0F, release_seconds * static_cast<float>(sample_rate));",
    "        const float release_step = 1.0F / std::max(1.0F, release_seconds * static_cast<float>(sample_rate));\n"
    "        const float fast_release_step = 1.0F / std::max(1.0F, 0.012F * static_cast<float>(sample_rate));",
    "fast release envelope step",
)
text = replace_once(
    text,
    "                } else {\n                    voice.env = std::max(0.0F, voice.env - release_step);\n                }\n                if (!voice.gate && voice.env <= kEpsilon) { voice.active = false; continue; }",
    "                } else {\n"
    "                    const float step = voice.fast_release ? fast_release_step : release_step;\n"
    "                    voice.env = std::max(0.0F, voice.env - step);\n"
    "                }\n"
    "                if (!voice.gate && voice.env <= kEpsilon) { voice.active = false; voice.fast_release = false; continue; }",
    "fast release envelope use",
)
text = replace_once(
    text,
    "void SynthEngine::play_chord(const std::vector<int>& notes) { std::scoped_lock lock(impl_->mutex); impl_->set_notes(notes); }\nvoid SynthEngine::release_chord()",
    "void SynthEngine::play_chord(const std::vector<int>& notes) { std::scoped_lock lock(impl_->mutex); impl_->set_notes(notes); }\n"
    "void SynthEngine::change_chord(const std::vector<int>& notes) { std::scoped_lock lock(impl_->mutex); impl_->change_notes(notes); }\n"
    "void SynthEngine::release_chord()",
    "public legato method",
)
text = replace_once(
    text,
    "    for (auto& voice : impl_->voices) { voice.active = false; voice.gate = false; voice.env = 0.0F; }",
    "    for (auto& voice : impl_->voices) { voice.active = false; voice.gate = false; voice.fast_release = false; voice.env = 0.0F; }",
    "all notes off fast flag",
)
path.write_text(text)


# Frontend legato routing and typography -------------------------------------
path = ROOT / "src/sdl_main.cpp"
text = path.read_text()

old_text_helpers = '''void text_right(SDL_Renderer* r, int right, int y, std::string_view value, int scale, Colour c, int tracking = 1) {
    text(r, right - text_width(value, scale, tracking), y, value, scale, c, false, tracking);
}
void micro(SDL_Renderer* r, int x, int y, std::string_view value, Colour c, bool centered = false) {
    text(r, x, y, value, 1, c, centered, 0);
}
'''
new_text_helpers = '''void text_right(SDL_Renderer* r, int right, int y, std::string_view value, int scale, Colour c, int tracking = 1) {
    text(r, right - text_width(value, scale, tracking), y, value, scale, c, false, tracking);
}
std::string fit_width(const std::string& value, int max_width, int scale = 1, int tracking = 1) {
    const int characters = brkchrd_ui::codepoint_count(value);
    for (int count = characters; count > 0; --count) {
        const std::string candidate = fit(value, count);
        if (text_width(candidate, scale, tracking) <= max_width) return candidate;
    }
    return {};
}
void text_box(SDL_Renderer* r, int x, int y, int w, int h, const std::string& value,
              int maximum_scale, Colour c, int tracking = 1) {
    int scale = std::max(1, maximum_scale);
    while (scale > 1 && text_width(value, scale, tracking) > w - 8) --scale;
    const std::string shown = fit_width(value, w - 8, scale, tracking);
    text(r, x + w / 2, y + (h - 7 * scale) / 2, shown, scale, c, true, tracking);
}
void micro(SDL_Renderer* r, int x, int y, std::string_view value, Colour c, bool centered = false) {
    text(r, x, y, value, 1, c, centered, 1);
}
void micro_right(SDL_Renderer* r, int right, int y, const std::string& value, Colour c) {
    text_right(r, right, y, value, 1, c, 1);
}
'''
text = replace_once(text, old_text_helpers, new_text_helpers, "text helpers")

old_draw_cell = '''void draw_cell(SDL_Renderer* r, int x, int y, int w, int h, const std::string& label,
               bool active, bool live, Colour accent) {
    Colour fill = active ? accent : live ? kPanel3 : kPanel2;
    rect(r, x, y, w, h, fill);
    outline(r, x, y, w, h, live ? kWhite : alpha(accent, 130));
    text(r, x + w / 2, y + h / 2 - 4, fit(label, 11), 1, active ? kBg : kInk, true, 0);
}
'''
new_draw_cell = '''void draw_cell(SDL_Renderer* r, int x, int y, int w, int h, const std::string& label,
               bool active, bool live, Colour accent) {
    Colour fill = active ? accent : live ? kPanel3 : kPanel2;
    rect(r, x, y, w, h, fill);
    outline(r, x, y, w, h, live ? kWhite : alpha(accent, 130));
    text_box(r, x + 3, y + 3, w - 6, h - 6, label, 2, active ? kBg : kInk, 1);
}
'''
text = replace_once(text, old_draw_cell, new_draw_cell, "readable dpad cells")

# Right-aligned status labels that previously used their right edge as a centre point.
text = text.replace(
    '''    micro(r, x + w - 12, y + 14,
          in.l1 ? brkchrd_ui::tr(ui.language, "L2 HELD", "L2 УДЕРЖ") : brkchrd_ui::tr(ui.language, "L2 ALT", "L2 АЛЬТ"),
          in.l1 ? kWhite : kDim, true);''',
    '''    micro_right(r, x + w - 12, y + 14,
                in.l1 ? brkchrd_ui::tr(ui.language, "L2 HELD", "L2 УДЕРЖ") : brkchrd_ui::tr(ui.language, "L2 ALT", "L2 АЛЬТ"),
                in.l1 ? kWhite : kDim);''',
)
text = text.replace(
    '''    micro(r, x + w - 12, y + 14,
          layer == 0 ? brkchrd_ui::tr(ui.language, "L2: ENV", "L2: ОГИБ") : brkchrd_ui::tr(ui.language, "L2 HELD", "L2 УДЕРЖ"),
          layer == 1 ? kWhite : kDim, true);''',
    '''    micro_right(r, x + w - 12, y + 14,
                layer == 0 ? brkchrd_ui::tr(ui.language, "L2: ENV", "L2: ОГИБ") : brkchrd_ui::tr(ui.language, "L2 HELD", "L2 УДЕРЖ"),
                layer == 1 ? kWhite : kDim);''',
)
text = text.replace(
    '''    micro(r, x + w - 12, y + 14,
          layer == 0 ? brkchrd_ui::tr(ui.language, "L2: ALT", "L2: АЛЬТ") : brkchrd_ui::tr(ui.language, "L2 HELD", "L2 УДЕРЖ"),
          layer == 1 ? kWhite : kDim, true);''',
    '''    micro_right(r, x + w - 12, y + 14,
                layer == 0 ? brkchrd_ui::tr(ui.language, "L2: ALT", "L2: АЛЬТ") : brkchrd_ui::tr(ui.language, "L2 HELD", "L2 УДЕРЖ"),
                layer == 1 ? kWhite : kDim);''',
)
text = replace_once(
    text,
    "    micro(r, x + w - 12, y + 14, fit(modifier, 17), kInk, true);",
    "    micro_right(r, x + w - 12, y + 14, fit_width(modifier, 102, 1, 1), kInk);",
    "chord modifier alignment",
)

old_preset_parameter = '''void preset_row(SDL_Renderer* r, int x, int y, int w, const std::string& value,
                bool active, Colour accent, Language language) {
    rect(r, x, y, w, 52, active ? accent : kPanel2);
    outline(r, x, y, w, 52, active ? kWhite : alpha(accent, 120));
    micro(r, x + 9, y + 7, brkchrd_ui::tr(language, "PRESET", "ПРЕСЕТ"), active ? kBg : kDim);
    text(r, x + w / 2, y + 24, fit(value, 17), 2, active ? kBg : kInk, true, 0);
}

void parameter_row(SDL_Renderer* r, int x, int y, int w, const std::string& name,
                   const std::string& value, bool active, Colour accent, float amount = -1.0F) {
    rect(r, x, y, w, 36, active ? accent : kPanel2);
    outline(r, x, y, w, 36, active ? kWhite : alpha(accent, 120));
    text(r, x + 9, y + 7, fit(name, 18), 1, active ? kBg : kInk, false, 0);
    text_right(r, x + w - 14, y + 7, fit(value, 17), 1, active ? kBg : kDim, 0);
    if (amount >= 0.0F) bar(r, x + 9, y + 25, w - 18, amount, active ? kWhite : accent);
}
'''
new_preset_parameter = '''void preset_row(SDL_Renderer* r, int x, int y, int w, const std::string& value,
                bool active, Colour accent, Language language) {
    rect(r, x, y, w, 52, active ? accent : kPanel2);
    outline(r, x, y, w, 52, active ? kWhite : alpha(accent, 120));
    micro(r, x + 9, y + 6, brkchrd_ui::tr(language, "PRESET", "ПРЕСЕТ"), active ? kBg : kDim);
    text_box(r, x + 6, y + 18, w - 12, 30, value, 2, active ? kBg : kInk, 1);
}

void parameter_row(SDL_Renderer* r, int x, int y, int w, const std::string& name,
                   const std::string& value, bool active, Colour accent, float amount = -1.0F) {
    rect(r, x, y, w, 36, active ? accent : kPanel2);
    outline(r, x, y, w, 36, active ? kWhite : alpha(accent, 120));
    const Colour label_colour = active ? kBg : kInk;
    const Colour value_colour = active ? kBg : kDim;
    text_box(r, x + 5, y + 2, w - 78, 21, name, 2, label_colour, 1);
    text_box(r, x + w - 70, y + 2, 65, 21, value, 2, value_colour, 1);
    if (amount >= 0.0F) bar(r, x + 9, y + 27, w - 18, amount, active ? kWhite : accent);
}
'''
text = replace_once(text, old_preset_parameter, new_preset_parameter, "preset and parameter typography")

old_chord_button = '''void chord_button(SDL_Renderer* r, int x, int y, int w, int h, const std::string& key,
                  const std::string& label, bool active, Colour accent, float pulse) {
    Colour fill = active ? accent : kPanel2;
    if (active && pulse > 0.5F) fill = kWhite;
    rect(r, x, y, w, h, fill); outline(r, x, y, w, h, active ? kWhite : alpha(accent, 140));
    text(r, x + 8, y + 7, key, 2, active && pulse <= 0.5F ? kBg : accent);
    text(r, x + w / 2, y + h - 17, fit(label, 12), 1, active && pulse <= 0.5F ? kBg : kInk, true, 0);
}
'''
new_chord_button = '''void chord_button(SDL_Renderer* r, int x, int y, int w, int h, const std::string& key,
                  const std::string& label, bool active, Colour accent, float pulse) {
    Colour fill = active ? accent : kPanel2;
    if (active && pulse > 0.5F) fill = kWhite;
    rect(r, x, y, w, h, fill); outline(r, x, y, w, h, active ? kWhite : alpha(accent, 140));
    text(r, x + 8, y + 6, key, 2, active && pulse <= 0.5F ? kBg : accent);
    text_box(r, x + 4, y + 27, w - 8, h - 31, label, 2, active && pulse <= 0.5F ? kBg : kInk, 1);
}
'''
text = replace_once(text, old_chord_button, new_chord_button, "chord button typography")

old_settings_rows = '''    text(r, 28, 28, brkchrd_ui::tr(ui.language, "SETTINGS", "НАСТРОЙКИ"), 3, kBlue);
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
'''
new_settings_rows = '''    text(r, 28, 28, brkchrd_ui::tr(ui.language, "SETTINGS", "НАСТРОЙКИ"), 3, kBlue);
    micro_right(r, 484, 38, brkchrd_ui::tr(ui.language, "SELECT: CLOSE", "SELECT: ЗАКРЫТЬ"), kDim);
    micro(r, 28, 58, fit_width(brkchrd_ui::tr(ui.language, "UP/DOWN SELECT  LEFT/RIGHT CHANGE", "ВВЕРХ/ВНИЗ: ВЫБОР  ВЛЕВО/ВПРАВО: ИЗМ."), 450, 1, 1), kDim);
    const int visible = 7;
    const int start = std::clamp(ui.settings_scroll, 0, kSettingsCount - visible);
    for (int i = 0; i < visible; ++i) {
        const int row = start + i; const bool active = row == ui.settings_row;
        const int row_y = 78 + i * 38;
        rect(r, 28, row_y, 456, 31, active ? kBlue : kPanel2);
        outline(r, 28, row_y, 456, 31, active ? kWhite : alpha(kBlue, 100));
        text_box(r, 34, row_y + 2, 274, 27, settings_name(row, ui), 2, active ? kBg : kInk, 1);
        text_box(r, 316, row_y + 2, 162, 27, settings_value(row, p, ui, synth), 2, active ? kBg : kDim, 1);
    }
    micro(r, 28, 352, brkchrd_ui::tr(ui.language, "START+SELECT: SAVE AND EXIT", "START+SELECT: СОХР. И ВЫХОД"), kDim);
    micro_right(r, 484, 352, std::to_string(ui.settings_row + 1) + "/" + std::to_string(kSettingsCount), kDim);
'''
text = replace_once(text, old_settings_rows, new_settings_rows, "settings typography")

old_footer = '''void draw_footer(SDL_Renderer* r, const UiState& ui, const InputState& in) {
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
'''
new_footer = '''void draw_footer(SDL_Renderer* r, const UiState& ui, const InputState& in) {
    rect(r, 0, 340, kWidth, 44, kPanel);
    std::string left_top;
    std::string left_bottom;
    if (ui.dpad_mode == DpadMode::Chord) {
        left_top = brkchrd_ui::tr(ui.language, "L2: ALT COLOUR", "L2: АЛЬТ. ОКРАСКА");
        left_bottom = brkchrd_ui::tr(ui.language, "L1: NEXT MODE", "L1: СЛЕД. РЕЖИМ");
    } else if (ui.dpad_mode == DpadMode::Sound) {
        left_top = brkchrd_ui::tr(ui.language, "DPAD: EDIT SOUND", "DPAD: ПРАВКА ЗВУКА");
        left_bottom = brkchrd_ui::tr(ui.language, "L2: SECOND BANK", "L2: ВТОРОЙ НАБОР");
    } else {
        left_top = brkchrd_ui::tr(ui.language, "HOLD DPAD: PERF FX", "УДЕРЖ DPAD: ПЕРФ FX");
        left_bottom = brkchrd_ui::tr(ui.language, "L2: SECOND FX BANK", "L2: ВТОРОЙ FX БАНК");
    }
    text_box(r, 8, 344, 240, 18, left_top, 1, kInk, 1);
    text_box(r, 8, 362, 240, 18, left_bottom, 1, kDim, 1);
    text_box(r, 264, 344, 240, 18, brkchrd_ui::tr(ui.language, "R1/R2: CHORD BANKS", "R1/R2: БАНКИ"), 1, kInk, 1);
    text_box(r, 264, 362, 240, 18, brkchrd_ui::tr(ui.language, "SELECT: SETUP  START: MODE", "SELECT: НАСТР.  START: РЕЖИМ"), 1, kDim, 1);
    if (in.l1) rect(r, 250, 347, 8, 8, kWhite);
}
'''
text = replace_once(text, old_footer, new_footer, "footer layout")

# Legato chord changes.
old_update = '''void update_chord(PerformanceState& p, UiState& ui, InputState& in,
                  std::optional<ChordSpec>& current, std::vector<int>& previous,
                  SynthEngine& synth) {
    if (!in.active_face) return;
    const int bank = active_bank(in, ui);
    p.chord_bank = bank;
    const int palette = effective_palette(in, ui);
    const Direction direction = effective_direction(in, ui);
    p.colour_palette = palette;
    const auto chord = make_chord(p.key_pc, p.octave, *in.active_face, bank, palette, direction);
    const auto notes = voice_chord(chord, previous, p.voice_leading, synth.voicing_profile());
    previous = notes; current = chord; synth.play_chord(notes);
}
'''
new_update = '''void update_chord(PerformanceState& p, UiState& ui, InputState& in,
                  std::optional<ChordSpec>& current, std::vector<int>& previous,
                  SynthEngine& synth, bool retrigger = true) {
    if (!in.active_face) return;
    const int bank = active_bank(in, ui);
    p.chord_bank = bank;
    const int palette = effective_palette(in, ui);
    const Direction direction = effective_direction(in, ui);
    p.colour_palette = palette;
    const auto chord = make_chord(p.key_pc, p.octave, *in.active_face, bank, palette, direction);
    const auto notes = voice_chord(chord, previous, p.voice_leading, synth.voicing_profile());
    previous = notes;
    current = chord;
    if (retrigger) synth.play_chord(notes);
    else synth.change_chord(notes);
}
'''
text = replace_once(text, old_update, new_update, "legato frontend API")

text = replace_once(text, "    if (in.active_face) update_chord(p, ui, in, current, previous, synth);\n}\n\nvoid cycle_dpad_mode", "    if (in.active_face) update_chord(p, ui, in, current, previous, synth, false);\n}\n\nvoid cycle_dpad_mode", "octave legato")
text = replace_once(text, "            if (in.active_face) update_chord(p, ui, in, current, previous, synth);\n            toast(ui, synth.preset_info().name);", "            if (in.active_face) update_chord(p, ui, in, current, previous, synth, false);\n            toast(ui, synth.preset_info().name);", "preset legato")
text = replace_once(text, "            if (in.active_face) update_chord(p, ui, in, current, previous, synth);\n        }\n    } else if (ui.dpad_mode == DpadMode::Sound)", "            if (in.active_face) update_chord(p, ui, in, current, previous, synth, false);\n        }\n    } else if (ui.dpad_mode == DpadMode::Sound)", "toggle colour legato")
text = replace_once(text, "    if (in.active_face) update_chord(p, ui, in, current, previous, synth);\n}\n\nvoid repeat_edit", "    if (in.active_face) update_chord(p, ui, in, current, previous, synth, false);\n}\n\nvoid repeat_edit", "hold colour legato")

old_rear = '''void set_r1(UiState& ui, PerformanceState& p, InputState& in, bool down,
            std::optional<ChordSpec>& current, std::vector<int>& previous, SynthEngine& synth) {
    if (in.r1 == down) return;
    in.r1 = down;
    if (in.active_face) update_chord(p, ui, in, current, previous, synth);
}
void set_r2(UiState& ui, PerformanceState& p, InputState& in, bool down,
            std::optional<ChordSpec>& current, std::vector<int>& previous, SynthEngine& synth) {
    if (in.r2 == down) return;
    in.r2 = down;
    if (in.active_face) update_chord(p, ui, in, current, previous, synth);
}

void set_left_physical(UiState& ui, PerformanceState& p, InputState& in, bool first, bool down,
                       std::optional<ChordSpec>& current, std::vector<int>& previous, SynthEngine& synth) {
    const bool physical_l1 = first != ui.swap_left_rear;
    if (physical_l1) set_l2(ui, in, down, synth); else set_l1(ui, in, down, synth);
    if (in.active_face) update_chord(p, ui, in, current, previous, synth);
}
'''
new_rear = '''void set_r1(UiState& ui, PerformanceState& p, InputState& in, bool down,
            std::optional<ChordSpec>& current, std::vector<int>& previous, SynthEngine& synth) {
    if (in.r1 == down) return;
    in.r1 = down;
    if (in.active_face) update_chord(p, ui, in, current, previous, synth, false);
}
void set_r2(UiState& ui, PerformanceState& p, InputState& in, bool down,
            std::optional<ChordSpec>& current, std::vector<int>& previous, SynthEngine& synth) {
    if (in.r2 == down) return;
    in.r2 = down;
    if (in.active_face) update_chord(p, ui, in, current, previous, synth, false);
}

void set_left_physical(UiState& ui, PerformanceState& p, InputState& in, bool first, bool down,
                       std::optional<ChordSpec>& current, std::vector<int>& previous, SynthEngine& synth) {
    const bool physical_l1 = first != ui.swap_left_rear;
    if (physical_l1) {
        set_l2(ui, in, down, synth);
        return;
    }
    set_l1(ui, in, down, synth);
    const bool changes_held_colour = ui.dpad_mode == DpadMode::Chord &&
        ui.chord_dpad == ChordDpadStyle::Hold && held_direction(in) != Direction::Center;
    if (changes_held_colour && in.active_face) update_chord(p, ui, in, current, previous, synth, false);
}
'''
text = replace_once(text, old_rear, new_rear, "rear controls without retrigger")

# Opening settings can change effective HOLD colour, but must not restart the envelope.
text = text.replace(
    "if (input.active_face) update_chord(state, ui, input, current, previous, synth); toast(ui, ui.settings",
    "if (input.active_face) update_chord(state, ui, input, current, previous, synth, false); toast(ui, ui.settings",
)
text = text.replace(
    "restore_performance_fx(ui, synth); ui.settings = !ui.settings; if (input.active_face) update_chord(state, ui, input, current, previous, synth); if (!ui.settings)",
    "restore_performance_fx(ui, synth); ui.settings = !ui.settings; if (input.active_face) update_chord(state, ui, input, current, previous, synth, false); if (!ui.settings)",
)

old_keyboard_l1 = '''                else if (key == SDLK_w) { set_l1(ui, input, down, synth); if (input.active_face) update_chord(state, ui, input, current, previous, synth); }'''
new_keyboard_l1 = '''                else if (key == SDLK_w) {
                    set_l1(ui, input, down, synth);
                    const bool changes_held_colour = ui.dpad_mode == DpadMode::Chord &&
                        ui.chord_dpad == ChordDpadStyle::Hold && held_direction(input) != Direction::Center;
                    if (changes_held_colour && input.active_face) update_chord(state, ui, input, current, previous, synth, false);
                }'''
text = replace_once(text, old_keyboard_l1, new_keyboard_l1, "keyboard alt layer")
path.write_text(text)


# Stress coverage ------------------------------------------------------------
path = ROOT / "tests/test_main.cpp"
text = path.read_text()
needle = '''    const auto cleared_delay = synth.render_copy(2048);
    CHECK(absolute_peak(cleared_delay) < 0.00001F);

    if (failures != 0) {
'''
replacement = '''    const auto cleared_delay = synth.render_copy(2048);
    CHECK(absolute_peak(cleared_delay) < 0.00001F);

    SynthEngine rapid(48000.0);
    rapid.set_preset(1);
    const std::array<std::vector<int>, 4> rapid_chords{{
        {48, 55, 60, 64}, {50, 57, 62, 65}, {52, 59, 64, 67}, {53, 60, 65, 69}
    }};
    for (int hit = 0; hit < 240; ++hit) {
        rapid.play_chord(rapid_chords[static_cast<std::size_t>(hit) % rapid_chords.size()]);
        const auto attack = rapid.render_copy(512);
        CHECK(std::all_of(attack.begin(), attack.end(), [](float sample) { return std::isfinite(sample); }));
        CHECK(absolute_peak(attack) <= 1.001F);
        rapid.release_chord();
        const auto gap = rapid.render_copy(128);
        CHECK(std::all_of(gap.begin(), gap.end(), [](float sample) { return std::isfinite(sample); }));
    }

    rapid.play_chord({48, 55, 60, 64});
    static_cast<void>(rapid.render_copy(1024));
    rapid.change_chord({48, 55, 60, 65});
    const auto legato = rapid.render_copy(2048);
    CHECK(std::all_of(legato.begin(), legato.end(), [](float sample) { return std::isfinite(sample); }));
    CHECK(absolute_peak(legato) <= 1.001F);

    if (failures != 0) {
'''
text = replace_once(text, needle, replacement, "rapid playing stress test")
path.write_text(text)

print("hardware feedback migration applied")
