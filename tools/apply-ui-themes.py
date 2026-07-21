from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return text.replace(old, new, 1)


sdl_path = ROOT / "src" / "sdl_main.cpp"
sdl = sdl_path.read_text()

old_colours = '''struct Colour { Uint8 r, g, b, a = 255; };
constexpr Colour kBg{8, 7, 12};
constexpr Colour kBgLift{15, 12, 22};
constexpr Colour kInk{238, 226, 197};
constexpr Colour kDim{143, 134, 153};
constexpr Colour kPurple{117, 67, 171};
constexpr Colour kPurpleBright{158, 99, 217};
constexpr Colour kPanel{31, 27, 42};
constexpr Colour kPanel2{45, 38, 57};
constexpr Colour kPanel3{60, 50, 74};
constexpr Colour kRed{216, 88, 88};
constexpr Colour kOrange{224, 154, 63};
constexpr Colour kTeal{80, 169, 154};
constexpr Colour kBlue{91, 122, 187};
constexpr Colour kGreen{112, 188, 120};
constexpr Colour kWhite{248, 242, 226};
'''
new_colours = '''struct Colour { Uint8 r, g, b, a = 255; };

struct UiTheme {
    const char* english;
    const char* russian;
    Colour bg;
    Colour bg_lift;
    Colour ink;
    Colour dim;
    Colour primary;
    Colour primary_bright;
    Colour panel;
    Colour panel2;
    Colour panel3;
    Colour red;
    Colour orange;
    Colour teal;
    Colour blue;
    Colour green;
    Colour white;
};

constexpr std::array<UiTheme, 5> kUiThemes{{
    {"AMETHYST", "АМЕТИСТ", {8,7,12}, {15,12,22}, {238,226,197}, {143,134,153},
     {117,67,171}, {158,99,217}, {31,27,42}, {45,38,57}, {60,50,74},
     {216,88,88}, {224,154,63}, {80,169,154}, {91,122,187}, {112,188,120}, {248,242,226}},
    {"LOGO", "ЛОГО", {17,21,20}, {27,31,29}, {241,227,197}, {155,148,132},
     {178,166,139}, {241,227,197}, {30,35,33}, {44,49,46}, {60,65,61},
     {201,115,98}, {210,160,92}, {120,170,151}, {119,145,171}, {142,170,113}, {255,246,226}},
    {"OCEAN", "ОКЕАН", {5,13,24}, {9,25,41}, {220,240,246}, {112,145,162},
     {22,101,140}, {48,181,205}, {12,31,48}, {18,47,68}, {26,64,88},
     {233,102,124}, {238,169,77}, {57,205,180}, {85,155,230}, {96,202,145}, {239,250,252}},
    {"EMBER", "УГЛИ", {20,10,8}, {35,17,12}, {248,226,194}, {162,125,105},
     {157,54,35}, {234,91,45}, {48,25,18}, {67,34,24}, {88,45,29},
     {235,72,56}, {244,151,49}, {132,178,120}, {110,139,176}, {167,191,102}, {255,239,211}},
    {"MONO", "МОНО", {7,8,10}, {17,18,21}, {231,233,236}, {128,132,138},
     {105,108,115}, {205,208,214}, {28,30,34}, {43,45,50}, {60,62,68},
     {183,185,190}, {159,162,168}, {137,140,146}, {117,120,126}, {196,198,202}, {250,250,250}}
}};
constexpr int kUiThemeCount = static_cast<int>(kUiThemes.size());

Colour kBg = kUiThemes[0].bg;
Colour kBgLift = kUiThemes[0].bg_lift;
Colour kInk = kUiThemes[0].ink;
Colour kDim = kUiThemes[0].dim;
Colour kPurple = kUiThemes[0].primary;
Colour kPurpleBright = kUiThemes[0].primary_bright;
Colour kPanel = kUiThemes[0].panel;
Colour kPanel2 = kUiThemes[0].panel2;
Colour kPanel3 = kUiThemes[0].panel3;
Colour kRed = kUiThemes[0].red;
Colour kOrange = kUiThemes[0].orange;
Colour kTeal = kUiThemes[0].teal;
Colour kBlue = kUiThemes[0].blue;
Colour kGreen = kUiThemes[0].green;
Colour kWhite = kUiThemes[0].white;

int normalise_theme(int index) {
    index %= kUiThemeCount;
    return index < 0 ? index + kUiThemeCount : index;
}

void apply_theme(int index) {
    const UiTheme& theme = kUiThemes[static_cast<std::size_t>(normalise_theme(index))];
    kBg = theme.bg; kBgLift = theme.bg_lift; kInk = theme.ink; kDim = theme.dim;
    kPurple = theme.primary; kPurpleBright = theme.primary_bright;
    kPanel = theme.panel; kPanel2 = theme.panel2; kPanel3 = theme.panel3;
    kRed = theme.red; kOrange = theme.orange; kTeal = theme.teal;
    kBlue = theme.blue; kGreen = theme.green; kWhite = theme.white;
}

std::string theme_name(Language language, int index) {
    const UiTheme& theme = kUiThemes[static_cast<std::size_t>(normalise_theme(index))];
    return language == Language::Russian ? theme.russian : theme.english;
}
'''
if sdl.count(old_colours) != 1:
    raise RuntimeError("theme colour system mismatch")
sdl = sdl.replace(old_colours, new_colours, 1)

replacements = [
    ("    ChordDpadStyle chord_dpad = ChordDpadStyle::Toggle;\n    bool settings = false;",
     "    ChordDpadStyle chord_dpad = ChordDpadStyle::Toggle;\n    int ui_theme = 0;\n    bool settings = false;", "ui theme state"),
    ('        else if (key == "chorddpad") ui.chord_dpad = static_cast<ChordDpadStyle>(std::clamp(static_cast<int>(value), 0, 1));\n        else if (key == "uimotion")',
     '        else if (key == "chorddpad") ui.chord_dpad = static_cast<ChordDpadStyle>(std::clamp(static_cast<int>(value), 0, 1));\n        else if (key == "uitheme") ui.ui_theme = std::clamp(static_cast<int>(value), 0, kUiThemeCount - 1);\n        else if (key == "uimotion")', "load theme"),
    ("    for (int slot = 0; slot < 2; ++slot) if (effect_seen[static_cast<std::size_t>(slot)]) synth.set_effect(slot, effects[static_cast<std::size_t>(slot)]);\n}",
     "    for (int slot = 0; slot < 2; ++slot) if (effect_seen[static_cast<std::size_t>(slot)]) synth.set_effect(slot, effects[static_cast<std::size_t>(slot)]);\n    apply_theme(ui.ui_theme);\n}", "apply loaded theme"),
    ("        << \"chorddpad \" << static_cast<int>(ui.chord_dpad) << '\\n'\n        << \"uimotion \" << ui.ui_motion << '\\n'",
     "        << \"chorddpad \" << static_cast<int>(ui.chord_dpad) << '\\n'\n        << \"uitheme \" << ui.ui_theme << '\\n'\n        << \"uimotion \" << ui.ui_motion << '\\n'", "save theme"),
    ("constexpr int kSettingsCount = 20;", "constexpr int kSettingsCount = 21;", "settings count"),
    ('        "LANGUAGE / ЯЗЫК", "CHORD DPAD"};', '        "LANGUAGE / ЯЗЫК", "CHORD DPAD", "UI PALETTE"};', "English theme row"),
    ('        "LANGUAGE / ЯЗЫК", "DPAD АККОРД"};', '        "LANGUAGE / ЯЗЫК", "DPAD АККОРД", "ПАЛИТРА UI"};', "Russian theme row"),
    ('    case 19: return brkchrd_ui::chord_dpad(ui.language, ui.chord_dpad);\n    default: return "";',
     '    case 19: return brkchrd_ui::chord_dpad(ui.language, ui.chord_dpad);\n    case 20: return theme_name(ui.language, ui.ui_theme);\n    default: return "";', "theme value"),
    ("    else if (row == 19) ui.chord_dpad = ui.chord_dpad == ChordDpadStyle::Toggle ? ChordDpadStyle::Hold : ChordDpadStyle::Toggle;\n}",
     "    else if (row == 19) ui.chord_dpad = ui.chord_dpad == ChordDpadStyle::Toggle ? ChordDpadStyle::Hold : ChordDpadStyle::Toggle;\n    else if (row == 20) { ui.ui_theme = normalise_theme(ui.ui_theme + delta); apply_theme(ui.ui_theme); }\n}", "theme edit"),
]
for old, new, label in replacements:
    sdl = replace_once(sdl, old, new, label)

sdl = replace_once(sdl, '''            toast(ui, ui.selected_direction == Direction::Center
                ? brkchrd_ui::tr(ui.language, "BASE", "ОСНОВА")
                : brkchrd_ui::palette(ui.language, ui.selected_palette) + " / " + brkchrd_ui::direction(ui.language, ui.selected_palette, ui.selected_direction));
''', "", "remove toggle chord toast")
sdl = replace_once(sdl, '''    toast(ui, direction == Direction::Center
        ? brkchrd_ui::tr(ui.language, "BASE", "ОСНОВА")
        : brkchrd_ui::palette(ui.language, palette) + " / " + brkchrd_ui::direction(ui.language, palette, direction));
''', "", "remove hold chord toast")

old_splash = '''bool show_splash(SDL_Renderer* r) {
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
new_splash = '''void draw_splash_logo(SDL_Renderer* r, Colour mark) {
    constexpr int x = 202, y = 48, w = 108, h = 132;
    outline(r, x, y, w, h, mark);
    outline(r, x + 2, y + 2, w - 4, h - 4, alpha(mark, 90));
    const std::array<int, 5> key_widths{12, 14, 15, 14, 11};
    int key_x = x + 14;
    for (int i = 0; i < 5; ++i) {
        const int key_h = 42 + (i % 2) * 8;
        rect(r, key_x, y + 16, key_widths[static_cast<std::size_t>(i)], key_h, mark);
        key_x += key_widths[static_cast<std::size_t>(i)] + 5;
    }
    rect(r, x + 20, y + 91, 27, 8, mark);
    rect(r, x + 29, y + 82, 8, 27, mark);
    rect(r, x + 60, y + 84, 8, 8, mark); rect(r, x + 76, y + 84, 8, 8, mark);
    rect(r, x + 60, y + 100, 8, 8, mark); rect(r, x + 76, y + 100, 8, 8, mark);
}

bool show_splash(SDL_Renderer* r, int theme_index) {
    if (std::getenv("BRKCHRD_SKIP_SPLASH") != nullptr) return true;
    apply_theme(theme_index);
    const Uint32 until = SDL_GetTicks() + 2000U;
    while (SDL_GetTicks() < until) {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) if (event.type == SDL_QUIT) return false;
        rect(r, 0, 0, kWidth, kHeight, kBg);
        for (int y = 0; y < kHeight; y += 24) line(r, 0, y, kWidth, y, alpha(kPurple, 18));
        draw_splash_logo(r, kPurpleBright);
        text(r, kWidth / 2, 206, "BRKCHRD", 4, kInk, true, 2);
        line(r, 148, 254, 364, 254, kPurpleBright);
        text(r, kWidth / 2, 280, "developed by myldy design", 1, kDim, true, 1);
        text(r, kWidth / 2, 306, "@myldy20", 2, kPurpleBright, true, 1);
        SDL_RenderPresent(r);
        SDL_Delay(16);
    }
    return true;
}
'''
sdl = replace_once(sdl, old_splash, new_splash, "theme splash")

sdl = replace_once(sdl, '''    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(renderer, kWidth, kHeight);
    if (!show_splash(renderer)) {
        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit(); return 0;
    }

    SynthEngine synth(kRate); PerformanceState state; UiState ui;
    load_config(state, ui, synth); synth.set_mode(state.mode); synth.set_bpm(state.bpm); synth.set_latch(false);
''', '''    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(renderer, kWidth, kHeight);

    SynthEngine synth(kRate); PerformanceState state; UiState ui;
    load_config(state, ui, synth); synth.set_mode(state.mode); synth.set_bpm(state.bpm); synth.set_latch(false);
    if (!show_splash(renderer, ui.ui_theme)) {
        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit(); return 0;
    }
''', "load theme before splash")

sdl_path.write_text(sdl)

for path in ROOT.rglob("*"):
    if not path.is_file() or ".git" in path.parts:
        continue
    if path.suffix.lower() not in {".md", ".cpp", ".hpp", ".h", ".sh", ".yml", ".yaml", ".txt"} and path.name != "CMakeLists.txt":
        continue
    try:
        text = path.read_text()
    except UnicodeDecodeError:
        continue
    updated = text.replace("0.5.2", "0.6.0").replace("v052", "v060")
    if updated != text:
        path.write_text(updated)

readme_path = ROOT / "README.md"
readme = readme_path.read_text()
readme = replace_once(readme,
    "- **Two chord-colour workflows.** `CHORD DPAD = TOGGLE` stores a colour; `HOLD` applies it only while the direction is held and returns to BASE on release.\n",
    "- **Two chord-colour workflows.** `CHORD DPAD = TOGGLE` stores a colour; `HOLD` applies it only while the direction is held and returns to BASE on release. The active colour is shown in the CHORD panel without covering the interface with a popup.\n- **Five interface palettes.** `UI PALETTE` switches AMETHYST, LOGO, OCEAN, EMBER and MONO immediately; the selected palette also colours the startup logo.\n", "README EN")
readme = replace_once(readme,
    "- **Два способа работы с окраской.** `DPAD АККОРД = НАЖАТИЕ` сохраняет её; `УДЕРЖ.` применяет только во время удержания и возвращает ОСНОВУ после отпускания.\n",
    "- **Два способа работы с окраской.** `DPAD АККОРД = НАЖАТИЕ` сохраняет её; `УДЕРЖ.` применяет только во время удержания и возвращает ОСНОВУ после отпускания. Активная окраска видна в панели АККОРД без перекрывающего интерфейс окна.\n- **Пять палитр интерфейса.** `ПАЛИТРА UI` сразу переключает АМЕТИСТ, ЛОГО, ОКЕАН, УГЛИ и МОНО; выбранная палитра используется и для логотипа при запуске.\n", "README RU")
readme_path.write_text(readme)

controls_en_path = ROOT / "docs" / "controls.en.md"
controls_en = controls_en_path.read_text()
controls_en = replace_once(controls_en,
    "HOLD does not overwrite the colour stored by TOGGLE. Returning to TOGGLE restores the stored selection.\n",
    "HOLD does not overwrite the colour stored by TOGGLE. Returning to TOGGLE restores the stored selection. The active colour is already shown below the left grid, so D-pad chord gestures do not open a separate popup.\n", "controls EN popup")
controls_en = replace_once(controls_en,
    "Important options include `VOICE LEAD`, `PERF FX`, `UI MOTION`, button swaps, `LANGUAGE / ЯЗЫК` and `CHORD DPAD`.\n",
    "Important options include `VOICE LEAD`, `PERF FX`, `UI MOTION`, button swaps, `LANGUAGE / ЯЗЫК`, `CHORD DPAD` and `UI PALETTE`.\n\n`UI PALETTE` offers AMETHYST, LOGO, OCEAN, EMBER and MONO. The change is immediate, is saved with the other settings and recolours the startup logo on the next launch. LOGO uses the project's original `#111514` charcoal and `#f1e3c5` cream as its anchor colours.\n", "controls EN themes")
controls_en_path.write_text(controls_en)

controls_ru_path = ROOT / "docs" / "controls.ru.md"
controls_ru = controls_ru_path.read_text()
controls_ru = replace_once(controls_ru,
    "УДЕРЖ. не стирает окраску, сохранённую режимом НАЖАТИЕ. При возврате к НАЖАТИЕ она снова станет активной.\n",
    "УДЕРЖ. не стирает окраску, сохранённую режимом НАЖАТИЕ. При возврате к НАЖАТИЕ она снова станет активной. Активная окраска уже показана под левой таблицей, поэтому отдельное всплывающее окно при жестах крестовины не появляется.\n", "controls RU popup")
controls_ru = replace_once(controls_ru,
    "Основные переключатели: ВЕДЕНИЕ, ПЕРФ FX, АНИМАЦИЯ, перестановка задних кнопок, `LANGUAGE / ЯЗЫК` и `DPAD АККОРД`.\n",
    "Основные переключатели: ВЕДЕНИЕ, ПЕРФ FX, АНИМАЦИЯ, перестановка задних кнопок, `LANGUAGE / ЯЗЫК`, `DPAD АККОРД` и `ПАЛИТРА UI`.\n\n`ПАЛИТРА UI` предлагает варианты АМЕТИСТ, ЛОГО, ОКЕАН, УГЛИ и МОНО. Оформление меняется сразу, сохраняется вместе с настройками и при следующем запуске перекрашивает заставку. ЛОГО использует исходные цвета проекта: угольный `#111514` и кремовый `#f1e3c5`.\n", "controls RU themes")
controls_ru_path.write_text(controls_ru)

manual_en_path = ROOT / "docs" / "manual.en.md"
manual_en = manual_en_path.read_text()
manual_en = replace_once(manual_en,
    "Switching back to TOGGLE restores the previously stored toggle colour; HOLD does not erase it.\n",
    "Switching back to TOGGLE restores the previously stored toggle colour; HOLD does not erase it. The selected colour remains visible below the left grid, so chord-colour gestures no longer produce a popup over the playable labels.\n", "manual EN popup")
manual_en = replace_once(manual_en, "## Stable and live voicing\n",
    "## Interface palettes\n\nSet `UI PALETTE` to AMETHYST, LOGO, OCEAN, EMBER or MONO. The complete interface changes immediately and the selection is saved. The two-second startup mark uses the same palette on the next launch. LOGO is based on the original BRKCHRD charcoal (`#111514`) and cream (`#f1e3c5`) artwork.\n\n## Stable and live voicing\n", "manual EN themes")
manual_en = replace_once(manual_en, "Settings contains twenty rows:", "Settings contains twenty-one rows:", "manual EN rows")
manual_en = replace_once(manual_en, "- UI Motion;\n- rear-button swaps;", "- UI Motion;\n- UI Palette;\n- rear-button swaps;", "manual EN bullet")
manual_en_path.write_text(manual_en)

manual_ru_path = ROOT / "docs" / "manual.ru.md"
manual_ru = manual_ru_path.read_text()
manual_ru = replace_once(manual_ru,
    "Режим УДЕРЖ. не стирает сохранённую окраску режима НАЖАТИЕ. При возврате к НАЖАТИЕ она снова станет активной.\n",
    "Режим УДЕРЖ. не стирает сохранённую окраску режима НАЖАТИЕ. При возврате к НАЖАТИЕ она снова станет активной. Выбранная окраска постоянно видна под левой таблицей, поэтому жесты крестовины больше не вызывают окно поверх полезных надписей.\n", "manual RU popup")
manual_ru = replace_once(manual_ru, "## Стабильное и живое голосоведение\n",
    "## Палитры интерфейса\n\nВ строке `ПАЛИТРА UI` доступны АМЕТИСТ, ЛОГО, ОКЕАН, УГЛИ и МОНО. Весь интерфейс меняется сразу, выбор сохраняется, а при следующем запуске та же палитра применяется к двухсекундной заставке. ЛОГО основана на исходном оформлении BRKCHRD: угольном `#111514` и кремовом `#f1e3c5`.\n\n## Стабильное и живое голосоведение\n", "manual RU themes")
manual_ru = manual_ru.replace("В настройках двадцать строк:", "В настройках двадцать одна строка:")
manual_ru = replace_once(manual_ru, "- анимация интерфейса;\n- перестановка задних кнопок;", "- анимация интерфейса;\n- палитра интерфейса;\n- перестановка задних кнопок;", "manual RU bullet")
manual_ru_path.write_text(manual_ru)

qa_en_path = ROOT / "docs" / "hardware-test-checklist.en.md"
qa_en = qa_en_path.read_text().replace("all 20 rows", "all 21 rows")
qa_en = qa_en.replace("`LANGUAGE / ЯЗЫК` and `CHORD DPAD` are reachable", "`LANGUAGE / ЯЗЫК`, `CHORD DPAD` and `UI PALETTE` are reachable")
qa_en = replace_once(qa_en, "- the language persists after restart;\n",
    "- the language persists after restart;\n- all five UI palettes apply immediately and persist; LOGO uses the charcoal/cream project colours;\n- the next startup logo uses the saved UI palette;\n", "QA EN themes")
qa_en = qa_en.replace("- pressing a direction stores its colour;", "- pressing a direction stores its colour without opening a popup over the chord labels;")
qa_en = qa_en.replace("- holding a direction applies its colour immediately;", "- holding a direction applies its colour immediately without opening a popup over the chord labels;")
qa_en_path.write_text(qa_en)

qa_ru_path = ROOT / "docs" / "hardware-test-checklist.ru.md"
qa_ru = qa_ru_path.read_text().replace("всех 20 строк", "всех 21 строки")
qa_ru = qa_ru.replace("доступны `LANGUAGE / ЯЗЫК` и `DPAD АККОРД`", "доступны `LANGUAGE / ЯЗЫК`, `DPAD АККОРД` и `ПАЛИТРА UI`")
qa_ru = replace_once(qa_ru, "- язык сохраняется после перезапуска;\n",
    "- язык сохраняется после перезапуска;\n- все пять палитр применяются сразу и сохраняются; ЛОГО использует угольный и кремовый цвета проекта;\n- при следующем запуске логотип использует сохранённую палитру;\n", "QA RU themes")
qa_ru = qa_ru.replace("- направление сохраняет окраску;", "- направление сохраняет окраску без окна поверх подписей аккордов;")
qa_ru = qa_ru.replace("- удержание направления сразу применяет окраску;", "- удержание направления сразу применяет окраску без окна поверх подписей аккордов;")
qa_ru_path.write_text(qa_ru)

workflow_path = ROOT / ".github" / "workflows" / "build.yml"
workflow = workflow_path.read_text()
workflow = replace_once(workflow,
'''      - name: SDL startup smoke test in Russian and hold-colour mode
        run: |
          mkdir -p /tmp/brkchrd-ru
          printf 'language 1\nchorddpad 1\n' > /tmp/brkchrd-ru/brkchrd.cfg
          set +e
          XDG_CONFIG_HOME=/tmp/brkchrd-ru BRKCHRD_SKIP_SPLASH=1 SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 3s build/brkchrd-sdl
          status=$?
          test "$status" -eq 0 -o "$status" -eq 124
''',
'''      - name: SDL startup smoke test in Russian and hold-colour mode
        run: |
          mkdir -p /tmp/brkchrd-ru
          printf 'language 1\nchorddpad 1\nuitheme 1\n' > /tmp/brkchrd-ru/brkchrd.cfg
          set +e
          XDG_CONFIG_HOME=/tmp/brkchrd-ru BRKCHRD_SKIP_SPLASH=1 SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 3s build/brkchrd-sdl
          status=$?
          test "$status" -eq 0 -o "$status" -eq 124
      - name: SDL startup smoke test for all UI palettes
        run: |
          for theme in 0 1 2 3 4; do
            dir="/tmp/brkchrd-theme-$theme"
            mkdir -p "$dir"
            printf 'uitheme %s\n' "$theme" > "$dir/brkchrd.cfg"
            set +e
            XDG_CONFIG_HOME="$dir" BRKCHRD_SKIP_SPLASH=1 SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 2s build/brkchrd-sdl
            status=$?
            set -e
            test "$status" -eq 0 -o "$status" -eq 124
          done
''', "theme smoke tests")
workflow = replace_once(workflow,
    "          grep -q 'CHORD DPAD' docs/controls.en.md\n          grep -q 'DPAD АККОРД' docs/controls.ru.md\n",
    "          grep -q 'CHORD DPAD' docs/controls.en.md\n          grep -q 'DPAD АККОРД' docs/controls.ru.md\n          grep -q 'UI PALETTE' docs/controls.en.md\n          grep -q 'ПАЛИТРА UI' docs/controls.ru.md\n          grep -q 'kUiThemes' src/sdl_main.cpp\n          grep -q 'uitheme' src/sdl_main.cpp\n", "theme CI checks")
workflow_path.write_text(workflow)
