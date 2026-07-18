
void draw_ui(SDL_Renderer* renderer,
             const PerformanceState& state,
             const InputState& input,
             const std::optional<ChordSpec>& current,
             const SynthEngine& synth,
             const std::string& toast) {
    fill_rect(renderer, 0, 0, kLogicalW, kLogicalH, kBg);
    fill_rect(renderer, 0, 0, kLogicalW, 42, kPanel);
    text(renderer, 14, 12, "BRKCHRD", 3, kAccent);
    text(renderer, 176, 8, "KEY " + note_name(state.key_pc), 2, kText);
    text(renderer, 176, 24, "OCT " + std::to_string(state.octave), 2, kDim);
    text(renderer, 266, 8, mode_name(state.mode), 2, kBlue);
    text(renderer, 266, 24, std::to_string(state.bpm) + " BPM", 2, kDim);
    const auto preset = synth.preset_info();
    text(renderer, 358, 8, preset.name, 2, kGreen);
    text(renderer, 358, 24, state.latch ? "LATCH ON" : "LATCH OFF", 2, state.latch ? kAccent : kDim);

    fill_rect(renderer, 12, 54, 176, 250, kPanel);
    text(renderer, 100, 64, palette_name(palette_from(input)), 2, palette_from(input) == 0 ? kDim : kAccent, true);
    const int cx = 100;
    const int cy = 178;
    fill_rect(renderer, cx - 24, cy - 18, 48, 36, kPanel2);
    text(renderer, cx, cy - 6, "BASE", 2, kText, true);
    const std::array<std::pair<Direction, std::pair<int,int>>, 8> dirs{{
        {Direction::Up, {cx, cy - 72}}, {Direction::UpRight, {cx + 56, cy - 52}},
        {Direction::Right, {cx + 64, cy}}, {Direction::DownRight, {cx + 54, cy + 52}},
        {Direction::Down, {cx, cy + 70}}, {Direction::DownLeft, {cx - 56, cy + 52}},
        {Direction::Left, {cx - 64, cy}}, {Direction::UpLeft, {cx - 56, cy - 52}},
    }};
    const Direction current_dir = direction_from(input);
    for (const auto& [dir, pos] : dirs) {
        const bool active = dir == current_dir;
        fill_rect(renderer, pos.first - 29, pos.second - 11, 58, 22, active ? kAccent : kPanel2);
        text(renderer, pos.first, pos.second - 5, direction_label(palette_from(input), dir), 1, active ? kBg : kText, true);
    }
    text(renderer, 100, 281, "L2 EXT  R2 DARK", 1, kDim, true);

    fill_rect(renderer, 200, 54, 300, 250, kPanel);
    text(renderer, 350, 64, bank_name(bank_from(input)), 2, bank_from(input) == 0 ? kDim : kAccent, true);
    draw_button(renderer, 320, 92, 98, 54, "Y", face_label(state.key_pc, Face::Y, bank_from(input)), input.face[3], kGreen);
    draw_button(renderer, 216, 151, 98, 54, "X", face_label(state.key_pc, Face::X, bank_from(input)), input.face[2], kBlue);
    draw_button(renderer, 398, 151, 90, 54, "B", face_label(state.key_pc, Face::B, bank_from(input)), input.face[1], kRed);
    draw_button(renderer, 320, 210, 98, 54, "A", face_label(state.key_pc, Face::A, bank_from(input)), input.face[0], kAccent);
    text(renderer, 350, 278, "L1 OTHER  R1 BORROW", 1, kDim, true);

    fill_rect(renderer, 12, 314, 488, 58, kPanel);
    if (current) {
        text(renderer, 26, 326, compact(current->degree + "  " + current->name), 4, kAccent);
        text(renderer, 286, 326, current->modifier, 2, kText);
        text(renderer, 286, 346, preset.character, 1, kDim);
    } else {
        text(renderer, 26, 326, "PLAY A B X Y", 3, kText);
        text(renderer, 286, 326, "DPAD KEY/OCT", 1, kDim);
        text(renderer, 286, 342, "START MODE  SELECT LATCH", 1, kDim);
        text(renderer, 286, 358, "L1+R1 NEXT SOUND", 1, kDim);
    }
    if (!toast.empty()) {
        fill_rect(renderer, 170, 146, 172, 44, kBg);
        line(renderer, 170, 146, 342, 146, kAccent);
        line(renderer, 170, 189, 342, 189, kAccent);
        text(renderer, 256, 160, toast, 2, kAccent, true);
    }
}

std::optional<Face> face_from_button(SDL_GameControllerButton button) {
    switch (button) {
        case SDL_CONTROLLER_BUTTON_A: return Face::A;
        case SDL_CONTROLLER_BUTTON_B: return Face::B;
        case SDL_CONTROLLER_BUTTON_X: return Face::X;
        case SDL_CONTROLLER_BUTTON_Y: return Face::Y;
        default: return std::nullopt;
    }
}

void update_current(PerformanceState& state,
                    InputState& input,
                    std::optional<ChordSpec>& current,
                    std::vector<int>& previous,
                    SynthEngine& synth) {
    state.chord_bank = bank_from(input);
    state.colour_palette = palette_from(input);
    if (!input.active_face) return;
    const ChordSpec chord = make_chord(state.key_pc, state.octave, *input.active_face,
                                       state.chord_bank, state.colour_palette, direction_from(input));
    const std::vector<int> notes = voice_lead(chord, previous);
    previous = notes;
    current = chord;
    synth.play_chord(notes);
}

void release_face(InputState& input,
                  Face face,
                  std::optional<ChordSpec>& current,
                  SynthEngine& synth) {
    input.face[static_cast<std::size_t>(face)] = false;
    if (input.active_face && *input.active_face == face) {
        input.active_face.reset();
        for (int i = 3; i >= 0; --i) {
            if (input.face[static_cast<std::size_t>(i)]) {
                input.active_face = static_cast<Face>(i);
                break;
            }
        }
        if (!input.active_face) {
            current.reset();
            synth.release_chord();
        }
    }
}

} // namespace

int main(int, char**) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_Window* window = SDL_CreateWindow("BRKCHRD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_RenderSetLogicalSize(renderer, kLogicalW, kLogicalH);

    PerformanceState state;
    load_config(state);
    SynthEngine synth(kAudioRate);
    synth.set_preset(state.preset);
    state.preset = synth.preset_index();
    synth.set_mode(state.mode);
    synth.set_bpm(state.bpm);
    synth.set_latch(state.latch);

    SDL_AudioSpec desired{};
    desired.freq = kAudioRate;
    desired.format = AUDIO_F32SYS;
    desired.channels = 2;
    desired.samples = kAudioFrames;
    desired.callback = audio_callback;
    desired.userdata = &synth;
    SDL_AudioSpec obtained{};
    SDL_AudioDeviceID audio = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (audio == 0) {
        std::cerr << "Audio failed: " << SDL_GetError() << '\n';
    } else {
        SDL_PauseAudioDevice(audio, 0);
    }

    SDL_GameController* controller = nullptr;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            if (controller) break;
        }
    }

    InputState input;
    std::optional<ChordSpec> current;
    std::vector<int> previous;
    bool running = true;
    std::string toast = "READY";
    Uint32 toast_until = SDL_GetTicks() + 900;
    bool sound_combo_latched = false;
    bool reverse_sound_combo_latched = false;

    auto show_toast = [&](const std::string& value) {
        toast = value;
        toast_until = SDL_GetTicks() + 900;
    };

    while (running) {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_CONTROLLERDEVICEADDED && !controller) controller = SDL_GameControllerOpen(event.cdevice.which);
            if (event.type == SDL_CONTROLLERDEVICEREMOVED && controller && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller)) == event.cdevice.which) {
                SDL_GameControllerClose(controller);
                controller = nullptr;
            }
            if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP) {
                const bool down = event.type == SDL_CONTROLLERBUTTONDOWN;
                const auto button = static_cast<SDL_GameControllerButton>(event.cbutton.button);
                if (auto face = face_from_button(button)) {
                    if (down) {
                        input.face[static_cast<std::size_t>(*face)] = true;
                        input.active_face = *face;
                        update_current(state, input, current, previous, synth);
                    } else {
                        release_face(input, *face, current, synth);
                        if (input.active_face) update_current(state, input, current, previous, synth);
                    }
                } else {
                    switch (button) {
                        case SDL_CONTROLLER_BUTTON_DPAD_UP: input.d_up = down; break;
                        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: input.d_down = down; break;
                        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: input.d_left = down; break;
                        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: input.d_right = down; break;
                        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: input.l1 = down; break;
                        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: input.r1 = down; break;
                        case SDL_CONTROLLER_BUTTON_START: input.start = down; break;
                        case SDL_CONTROLLER_BUTTON_BACK: input.select = down; break;
                        default: break;
                    }
                    if (input.start && input.select) running = false;
                    if (!down && button == SDL_CONTROLLER_BUTTON_START && !input.select) {
                        state.mode = static_cast<PlayMode>((static_cast<int>(state.mode) + 1) % 4);
                        synth.set_mode(state.mode);
                        show_toast(mode_name(state.mode));
                    }
                    if (!down && button == SDL_CONTROLLER_BUTTON_BACK && !input.start) {
                        state.latch = !state.latch;
                        synth.set_latch(state.latch);
                        show_toast(state.latch ? "LATCH ON" : "LATCH OFF");
