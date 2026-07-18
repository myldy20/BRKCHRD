                    }
                    if (input.active_face) {
                        update_current(state, input, current, previous, synth);
                    } else if (down && (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT || button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT ||
                                             button == SDL_CONTROLLER_BUTTON_DPAD_UP || button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
                        if (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) state.key_pc = (state.key_pc + 11) % 12;
                        if (button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) state.key_pc = (state.key_pc + 1) % 12;
                        if (button == SDL_CONTROLLER_BUTTON_DPAD_UP) state.octave = std::min(2, state.octave + 1);
                        if (button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) state.octave = std::max(-2, state.octave - 1);
                        show_toast("KEY " + note_name(state.key_pc) + " OCT " + std::to_string(state.octave));
                    }
                }
            }
            if (event.type == SDL_CONTROLLERAXISMOTION) {
                const bool held = event.caxis.value > 15000;
                if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT && input.l2 != held) {
                    input.l2 = held;
                    if (input.active_face) update_current(state, input, current, previous, synth);
                }
                if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT && input.r2 != held) {
                    input.r2 = held;
                    if (input.active_face) update_current(state, input, current, previous, synth);
                }
            }
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                const bool down = event.type == SDL_KEYDOWN;
                const SDL_Keycode key = event.key.keysym.sym;
                auto set_face = [&](Face face) {
                    if (down) {
                        input.face[static_cast<std::size_t>(face)] = true;
                        input.active_face = face;
                        update_current(state, input, current, previous, synth);
                    } else {
                        release_face(input, face, current, synth);
                    }
                };
                if (key == SDLK_z) set_face(Face::A);
                else if (key == SDLK_x) set_face(Face::B);
                else if (key == SDLK_a) set_face(Face::X);
                else if (key == SDLK_s) set_face(Face::Y);
                else if (key == SDLK_UP) input.d_up = down;
                else if (key == SDLK_DOWN) input.d_down = down;
                else if (key == SDLK_LEFT) input.d_left = down;
                else if (key == SDLK_RIGHT) input.d_right = down;
                else if (key == SDLK_q) input.l1 = down;
                else if (key == SDLK_w) input.r1 = down;
                else if (key == SDLK_e) input.l2 = down;
                else if (key == SDLK_r) input.r2 = down;
                else if (key == SDLK_ESCAPE && down) running = false;
                else if (key == SDLK_RETURN && down && event.key.repeat == 0) {
                    state.mode = static_cast<PlayMode>((static_cast<int>(state.mode) + 1) % 4);
                    synth.set_mode(state.mode);
                    show_toast(mode_name(state.mode));
                } else if (key == SDLK_SPACE && down && event.key.repeat == 0) {
                    state.latch = !state.latch;
                    synth.set_latch(state.latch);
                    show_toast(state.latch ? "LATCH ON" : "LATCH OFF");
                } else if (key == SDLK_LEFTBRACKET && down) {
                    state.preset = (state.preset + SynthEngine::preset_count() - 1) % SynthEngine::preset_count();
                    synth.set_preset(state.preset);
                    show_toast(synth.preset_info().name);
                } else if (key == SDLK_RIGHTBRACKET && down) {
                    state.preset = (state.preset + 1) % SynthEngine::preset_count();
                    synth.set_preset(state.preset);
                    show_toast(synth.preset_info().name);
                }
                if (input.active_face && (key == SDLK_UP || key == SDLK_DOWN || key == SDLK_LEFT || key == SDLK_RIGHT ||
                                          key == SDLK_q || key == SDLK_w || key == SDLK_e || key == SDLK_r)) {
                    update_current(state, input, current, previous, synth);
                } else if (!input.active_face && down && event.key.repeat == 0) {
                    if (key == SDLK_LEFT) state.key_pc = (state.key_pc + 11) % 12;
                    if (key == SDLK_RIGHT) state.key_pc = (state.key_pc + 1) % 12;
                    if (key == SDLK_UP) state.octave = std::min(2, state.octave + 1);
                    if (key == SDLK_DOWN) state.octave = std::max(-2, state.octave - 1);
                }
            }
        }

        if (input.l1 && input.r1 && !input.active_face) {
            if (!sound_combo_latched) {
                state.preset = (state.preset + 1) % SynthEngine::preset_count();
                synth.set_preset(state.preset);
                show_toast(synth.preset_info().name);
                sound_combo_latched = true;
            }
        } else sound_combo_latched = false;
        if (input.l2 && input.r2 && !input.active_face) {
            if (!reverse_sound_combo_latched) {
                state.preset = (state.preset + SynthEngine::preset_count() - 1) % SynthEngine::preset_count();
                synth.set_preset(state.preset);
                show_toast(synth.preset_info().name);
                reverse_sound_combo_latched = true;
            }
        } else reverse_sound_combo_latched = false;

        if (SDL_GetTicks() > toast_until) toast.clear();
        draw_ui(renderer, state, input, current, synth, toast);
        SDL_RenderPresent(renderer);
    }

    save_config(state);
    synth.all_notes_off();
    if (audio != 0) SDL_CloseAudioDevice(audio);
    if (controller) SDL_GameControllerClose(controller);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
