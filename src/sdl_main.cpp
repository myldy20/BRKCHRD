#define SDL_MAIN_HANDLED
#include "brkchrd/music.hpp"
#include "brkchrd/synth.hpp"

#include <SDL.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace brkchrd;

namespace {

constexpr int kWidth = 512;
constexpr int kHeight = 384;
constexpr int kRate = 48000;
constexpr int kFrames = 512;
constexpr const char* kVersion = "0.3.0";

struct Colour { Uint8 r, g, b, a = 255; };
constexpr Colour kBg{8, 7, 12};
constexpr Colour kBgLift{15, 12, 22};
constexpr Colour kInk{238, 226, 197};
constexpr Colour kDim{144, 135, 151};
constexpr Colour kPurple{117, 67, 171};
constexpr Colour kPurpleBright{154, 96, 213};
constexpr Colour kPanel{31, 27, 42};
constexpr Colour kPanel2{44, 38, 56};
constexpr Colour kPanel3{58, 49, 72};
constexpr Colour kRed{216, 88, 88};
constexpr Colour kOrange{224, 154, 63};
constexpr Colour kTeal{80, 169, 154};
constexpr Colour kBlue{91, 122, 187};
constexpr Colour kGreen{112, 188, 120};
constexpr Colour kWhite{248, 242, 226};

void colour(SDL_Renderer* renderer, Colour value) {
    SDL_SetRenderDrawColor(renderer, value.r, value.g, value.b, value.a);
}
void rect(SDL_Renderer* renderer, int x, int y, int w, int h, Colour value) {
    colour(renderer, value);
    SDL_Rect area{x, y, w, h};
    SDL_RenderFillRect(renderer, &area);
}
void outline(SDL_Renderer* renderer, int x, int y, int w, int h, Colour value) {
    colour(renderer, value);
    SDL_Rect area{x, y, w, h};
    SDL_RenderDrawRect(renderer, &area);
}
void line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, Colour value) {
    colour(renderer, value);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}
Colour alpha(Colour value, Uint8 a) {
    value.a = a;
    return value;
}

std::array<std::uint8_t, 7> glyph(char value) {
    const char ch = static_cast<char>(std::toupper(static_cast<unsigned char>(value)));
    switch (ch) {
    case 'A': return {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}; case 'B': return {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E};
    case 'C': return {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}; case 'D': return {0x1E,0x11,0x11,0x11,0x11,0x11,0x1E};
    case 'E': return {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}; case 'F': return {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10};
    case 'G': return {0x0E,0x11,0x10,0x17,0x11,0x11,0x0E}; case 'H': return {0x11,0x11,0x11,0x1F,0x11,0x11,0x11};
    case 'I': return {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}; case 'J': return {0x07,0x02,0x02,0x02,0x12,0x12,0x0C};
    case 'K': return {0x11,0x12,0x14,0x18,0x14,0x12,0x11}; case 'L': return {0x10,0x10,0x10,0x10,0x10,0x10,0x1F};
    case 'M': return {0x11,0x1B,0x15,0x15,0x11,0x11,0x11}; case 'N': return {0x11,0x19,0x15,0x13,0x11,0x11,0x11};
    case 'O': return {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}; case 'P': return {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10};
    case 'Q': return {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}; case 'R': return {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11};
    case 'S': return {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}; case 'T': return {0x1F,0x04,0x04,0x04,0x04,0x04,0x04};
    case 'U': return {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}; case 'V': return {0x11,0x11,0x11,0x11,0x11,0x0A,0x04};
    case 'W': return {0x11,0x11,0x11,0x15,0x15,0x15,0x0A}; case 'X': return {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11};
    case 'Y': return {0x11,0x11,0x0A,0x04,0x04,0x04,0x04}; case 'Z': return {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F};
    case '0': return {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}; case '1': return {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E};
    case '2': return {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F}; case '3': return {0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E};
    case '4': return {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}; case '5': return {0x1F,0x10,0x10,0x1E,0x01,0x01,0x1E};
    case '6': return {0x0E,0x10,0x10,0x1E,0x11,0x11,0x0E}; case '7': return {0x1F,0x01,0x02,0x04,0x08,0x08,0x08};
    case '8': return {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}; case '9': return {0x0E,0x11,0x11,0x0F,0x01,0x01,0x0E};
    case '#': return {0x0A,0x1F,0x0A,0x0A,0x1F,0x0A,0x00}; case '+': return {0x00,0x04,0x04,0x1F,0x04,0x04,0x00};
    case '-': return {0x00,0x00,0x00,0x1F,0x00,0x00,0x00}; case '/': return {0x01,0x02,0x02,0x04,0x08,0x08,0x10};
    case '.': return {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C}; case ':': return {0x00,0x0C,0x0C,0x00,0x0C,0x0C,0x00};
    case '<': return {0x02,0x04,0x08,0x10,0x08,0x04,0x02}; case '>': return {0x08,0x04,0x02,0x01,0x02,0x04,0x08};
    case '[': return {0x0E,0x08,0x08,0x08,0x08,0x08,0x0E}; case ']': return {0x0E,0x02,0x02,0x02,0x02,0x02,0x0E};
    case '%': return {0x19,0x19,0x02,0x04,0x08,0x13,0x13}; case '=': return {0x00,0x1F,0x00,0x1F,0x00,0x00,0x00};
    case '_': return {0x00,0x00,0x00,0x00,0x00,0x00,0x1F}; case ' ': return {0,0,0,0,0,0,0};
    default: return {0x1F,0x11,0x02,0x04,0x00,0x04,0x00};
    }
}

int text_width(std::string_view value, int scale = 1, int tracking = 1) {
    if (value.empty()) return 0;
    return static_cast<int>(value.size()) * (5 * scale + tracking) - tracking;
}
std::string fit(std::string value, int chars) {
    if (chars < 1) return {};
    if (static_cast<int>(value.size()) <= chars) return value;
    if (chars <= 3) return value.substr(0, static_cast<std::size_t>(chars));
    return value.substr(0, static_cast<std::size_t>(chars - 3)) + "...";
}
void text(SDL_Renderer* renderer, int x, int y, std::string_view value, int scale, Colour value_colour,
          bool centered = false, int tracking = 1) {
    if (centered) x -= text_width(value, scale, tracking) / 2;
    colour(renderer, value_colour);
    int cursor = x;
    for (char ch : value) {
        const auto rows = glyph(ch);
        for (int row = 0; row < 7; ++row) {
            for (int column = 0; column < 5; ++column) {
                if (((rows[static_cast<std::size_t>(row)] >> (4 - column)) & 1U) != 0U) {
                    SDL_Rect pixel{cursor + column * scale, y + row * scale, scale, scale};
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        cursor += 5 * scale + tracking;
    }
}
void micro(SDL_Renderer* renderer, int x, int y, std::string_view value, Colour value_colour, bool centered = false) {
    text(renderer, x, y, value, 1, value_colour, centered, 0);
}
void bar(SDL_Renderer* renderer, int x, int y, int w, float value, Colour active) {
    rect(renderer, x, y, w, 5, kBg);
    outline(renderer, x, y, w, 5, alpha(kDim, 150));
    rect(renderer, x + 1, y + 1,
         static_cast<int>(static_cast<float>(w - 2) * std::clamp(value, 0.0F, 1.0F)), 3, active);
}

enum class LeftMode { Classic = 0, Extended, Dark, Sound, System, Count };
enum class RightMode { Core = 0, Diatonic, Borrowed, Fx1, Fx2, Master, Count };
enum class Focus { Left = 0, Right };

struct InputState {
    std::array<bool, 4> face{};
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool front_left = false;
    bool front_right = false;
    bool rear_left = false;
    bool rear_right = false;
    bool start = false;
    bool select = false;
    bool select_long = false;
    Uint32 select_since = 0U;
    std::optional<Face> active_face;
};

struct UiState {
    LeftMode left_mode = LeftMode::Classic;
    RightMode right_mode = RightMode::Core;
    Focus focus = Focus::Left;
    Direction armed = Direction::Center;
    int palette = 0;
    int bank = 0;
    int sound_row = 0;
    int system_row = 0;
    std::array<int, 3> fx_row{};
    int ui_motion = 2;
    std::string toast;
    Uint32 toast_until = 0U;
    int held_edit = 0;
    Uint32 held_since = 0U;
    Uint32 last_repeat = 0U;
    Uint32 last_left_cycle = 0U;
    Uint32 last_right_cycle = 0U;
};

std::filesystem::path config_path() {
    if (const char* root = std::getenv("XDG_CONFIG_HOME")) return std::filesystem::path(root) / "brkchrd.cfg";
    if (const char* home = std::getenv("HOME")) return std::filesystem::path(home) / ".config" / "brkchrd.cfg";
    return "brkchrd.cfg";
}

void toast(UiState& ui, std::string value, Uint32 duration = 1000U) {
    ui.toast = std::move(value);
    ui.toast_until = SDL_GetTicks() + duration;
}

int palette_for(LeftMode mode, int fallback) {
    if (mode == LeftMode::Classic) return 0;
    if (mode == LeftMode::Extended) return 1;
    if (mode == LeftMode::Dark) return 2;
    return fallback;
}
bool colour_mode(LeftMode mode) {
    return mode == LeftMode::Classic || mode == LeftMode::Extended || mode == LeftMode::Dark;
}
bool chord_mode(RightMode mode) {
    return mode == RightMode::Core || mode == RightMode::Diatonic || mode == RightMode::Borrowed;
}
int bank_for(RightMode mode, int fallback) {
    if (mode == RightMode::Core) return 0;
    if (mode == RightMode::Diatonic) return 1;
    if (mode == RightMode::Borrowed) return 2;
    return fallback;
}
std::string left_mode_name(LeftMode mode) {
    switch (mode) {
    case LeftMode::Classic: return "CLASSIC";
    case LeftMode::Extended: return "EXTENDED";
    case LeftMode::Dark: return "DARK";
    case LeftMode::Sound: return "SOUND";
    case LeftMode::System: return "SYSTEM";
    case LeftMode::Count: break;
    }
    return "CLASSIC";
}
std::string right_mode_name(RightMode mode) {
    switch (mode) {
    case RightMode::Core: return "CORE";
    case RightMode::Diatonic: return "DIATONIC+";
    case RightMode::Borrowed: return "BORROWED";
    case RightMode::Fx1: return "FX 1";
    case RightMode::Fx2: return "FX 2";
    case RightMode::Master: return "MASTER";
    case RightMode::Count: break;
    }
    return "CORE";
}
Colour left_accent(LeftMode mode) {
    if (mode == LeftMode::Sound) return kOrange;
    if (mode == LeftMode::System) return kBlue;
    if (mode == LeftMode::Dark) return kRed;
    if (mode == LeftMode::Extended) return kPurpleBright;
    return kPurple;
}
Colour right_accent(RightMode mode) {
    if (mode == RightMode::Fx1) return kRed;
    if (mode == RightMode::Fx2) return kOrange;
    if (mode == RightMode::Master) return kTeal;
    if (mode == RightMode::Borrowed) return kBlue;
    if (mode == RightMode::Diatonic) return kGreen;
    return kPurple;
}

Direction held_direction(const InputState& input) {
    if (input.up && input.left) return Direction::UpLeft;
    if (input.up && input.right) return Direction::UpRight;
    if (input.down && input.left) return Direction::DownLeft;
    if (input.down && input.right) return Direction::DownRight;
    if (input.up) return Direction::Up;
    if (input.right) return Direction::Right;
    if (input.down) return Direction::Down;
    if (input.left) return Direction::Left;
    return Direction::Center;
}
Direction performance_direction(const InputState& input, const UiState& ui) {
    const Direction live = held_direction(input);
    return live == Direction::Center ? ui.armed : live;
}

void load_config(PerformanceState& performance, UiState& ui, SynthEngine& synth) {
    std::ifstream in(config_path());
    std::string key;
    float value = 0.0F;
    std::array<float, static_cast<std::size_t>(SynthParameter::Count)> params{};
    std::array<bool, static_cast<std::size_t>(SynthParameter::Count)> param_seen{};
    std::array<EffectSettings, 2> effects{};
    std::array<bool, 2> effect_seen{};
    while (in >> key >> value) {
        if (key == "key") performance.key_pc = std::clamp(static_cast<int>(value), 0, 11);
        else if (key == "octave") performance.octave = std::clamp(static_cast<int>(value), -2, 2);
        else if (key == "preset") performance.preset = static_cast<int>(value);
        else if (key == "bpm") performance.bpm = std::clamp(static_cast<int>(value), 40, 240);
        else if (key == "mode") performance.mode = static_cast<PlayMode>(std::clamp(static_cast<int>(value), 0, 3));
        else if (key == "latch") performance.latch = value != 0.0F;
        else if (key == "palette") ui.palette = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "bank") ui.bank = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "direction") ui.armed = static_cast<Direction>(std::clamp(static_cast<int>(value), 0, 8));
        else if (key == "leftmode") ui.left_mode = static_cast<LeftMode>(std::clamp(static_cast<int>(value), 0, 4));
        else if (key == "rightmode") ui.right_mode = static_cast<RightMode>(std::clamp(static_cast<int>(value), 0, 5));
        else if (key == "uimotion") ui.ui_motion = std::clamp(static_cast<int>(value), 0, 2);
        else if (key.rfind("param", 0) == 0) {
            const int index = std::clamp(std::stoi(key.substr(5)), 0, static_cast<int>(SynthParameter::Count) - 1);
            params[static_cast<std::size_t>(index)] = value;
            param_seen[static_cast<std::size_t>(index)] = true;
        } else if (key == "fx1type") { effects[0].type = static_cast<EffectType>(std::clamp(static_cast<int>(value), 0, SynthEngine::effect_count() - 1)); effect_seen[0] = true; }
        else if (key == "fx1amount") { effects[0].amount = value; effect_seen[0] = true; }
        else if (key == "fx1colour") { effects[0].colour = value; effect_seen[0] = true; }
        else if (key == "fx2type") { effects[1].type = static_cast<EffectType>(std::clamp(static_cast<int>(value), 0, SynthEngine::effect_count() - 1)); effect_seen[1] = true; }
        else if (key == "fx2amount") { effects[1].amount = value; effect_seen[1] = true; }
        else if (key == "fx2colour") { effects[1].colour = value; effect_seen[1] = true; }
    }
    synth.set_preset(performance.preset);
    performance.preset = synth.preset_index();
    for (int i = 0; i < static_cast<int>(SynthParameter::Count); ++i) {
        if (param_seen[static_cast<std::size_t>(i)]) {
            synth.set_parameter(static_cast<SynthParameter>(i), params[static_cast<std::size_t>(i)]);
        }
    }
    for (int slot = 0; slot < 2; ++slot) {
        if (effect_seen[static_cast<std::size_t>(slot)]) synth.set_effect(slot, effects[static_cast<std::size_t>(slot)]);
    }
    ui.palette = palette_for(ui.left_mode, ui.palette);
    ui.bank = bank_for(ui.right_mode, ui.bank);
}

void save_config(const PerformanceState& performance, const UiState& ui, const SynthEngine& synth) {
    const auto path = config_path();
    std::error_code error;
    std::filesystem::create_directories(path.parent_path(), error);
    std::ofstream out(path);
    out << "key " << performance.key_pc << '\n'
        << "octave " << performance.octave << '\n'
        << "preset " << synth.preset_index() << '\n'
        << "bpm " << performance.bpm << '\n'
        << "mode " << static_cast<int>(performance.mode) << '\n'
        << "latch " << (performance.latch ? 1 : 0) << '\n'
        << "palette " << ui.palette << '\n'
        << "bank " << ui.bank << '\n'
        << "direction " << static_cast<int>(ui.armed) << '\n'
        << "leftmode " << static_cast<int>(ui.left_mode) << '\n'
        << "rightmode " << static_cast<int>(ui.right_mode) << '\n'
        << "uimotion " << ui.ui_motion << '\n';
    for (int i = 0; i < static_cast<int>(SynthParameter::Count); ++i) {
        out << "param" << i << ' ' << synth.parameter(static_cast<SynthParameter>(i)) << '\n';
    }
    for (int slot = 0; slot < 2; ++slot) {
        const auto fx = synth.effect(slot);
        const int number = slot + 1;
        out << "fx" << number << "type " << static_cast<int>(fx.type) << '\n'
            << "fx" << number << "amount " << fx.amount << '\n'
            << "fx" << number << "colour " << fx.colour << '\n';
    }
}

void audio_callback(void* userdata, Uint8* bytes, int count) {
    auto* synth = static_cast<SynthEngine*>(userdata);
    auto* output = reinterpret_cast<float*>(bytes);
    synth->render(output, static_cast<std::size_t>(count) / (sizeof(float) * 2U));
}

void draw_background(SDL_Renderer* renderer, const UiState& ui, float peak) {
    rect(renderer, 0, 0, kWidth, kHeight, kBg);
    if (ui.ui_motion == 0) return;
    const float time = static_cast<float>(SDL_GetTicks()) * 0.001F;
    const int drift = ui.ui_motion == 2 ? static_cast<int>(std::fmod(time * 9.0F, 24.0F)) : 0;
    for (int x = -24 + drift; x < kWidth + 24; x += 24) {
        line(renderer, x, 36, x - 58, kHeight, alpha(kPurple, 22));
    }
    for (int y = 42; y < kHeight; y += 24) {
        line(renderer, 0, y, kWidth, y, alpha(kInk, 12));
    }
    if (ui.ui_motion == 2) {
        const int dots = 14;
        for (int i = 0; i < dots; ++i) {
            const float phase = time * (0.35F + static_cast<float>(i % 4) * 0.07F) + static_cast<float>(i) * 1.73F;
            const int x = static_cast<int>(std::fmod(phase * 41.0F + static_cast<float>(i * 53), static_cast<float>(kWidth)));
            const int y = 42 + static_cast<int>(std::fmod(phase * 19.0F + static_cast<float>(i * 37), 270.0F));
            const int size = 1 + (i % 2);
            rect(renderer, x, y, size, size, alpha(i % 3 == 0 ? kTeal : kPurpleBright,
                                                   static_cast<Uint8>(35 + peak * 90.0F)));
        }
    }
}

void draw_header(SDL_Renderer* renderer, const PerformanceState& state, const UiState& ui, const SynthEngine& synth) {
    rect(renderer, 0, 0, kWidth, 36, alpha(kPanel, 245));
    line(renderer, 0, 35, kWidth, 35, alpha(kPurpleBright, 120));
    text(renderer, 12, 8, "BRKCHRD", 2, kInk, false, 1);
    micro(renderer, 129, 8, "KEY", kDim);
    text(renderer, 129, 18, note_name(state.key_pc), 1, kWhite, false, 1);
    micro(renderer, 170, 8, "OCT", kDim);
    text(renderer, 170, 18, (state.octave >= 0 ? "+" : "") + std::to_string(state.octave), 1, kWhite, false, 1);
    micro(renderer, 216, 8, "MODE", kDim);
    text(renderer, 216, 18, fit(mode_name(state.mode), 7), 1, kOrange, false, 1);
    micro(renderer, 279, 8, "LATCH", kDim);
    text(renderer, 279, 18, state.latch ? "ON" : "OFF", 1, state.latch ? kGreen : kDim, false, 1);
    micro(renderer, 334, 8, "LEFT", kDim);
    text(renderer, 334, 18, fit(left_mode_name(ui.left_mode), 10), 1, left_accent(ui.left_mode), false, 1);
    micro(renderer, 416, 8, "RIGHT", kDim);
    text(renderer, 416, 18, fit(right_mode_name(ui.right_mode), 12), 1, right_accent(ui.right_mode), false, 1);
    const float peak = synth.output_peak();
    rect(renderer, 482, 8, 18, 19, kBg);
    rect(renderer, 485, 24 - static_cast<int>(14.0F * std::clamp(peak, 0.0F, 1.0F)),
         12, static_cast<int>(14.0F * std::clamp(peak, 0.0F, 1.0F)),
         peak > 0.82F ? kRed : kTeal);
}

void panel_frame(SDL_Renderer* renderer, int x, int y, int w, int h, bool focused, Colour accent,
                 const std::string& title, const std::string& side_hint) {
    rect(renderer, x, y, w, h, alpha(kPanel, 246));
    outline(renderer, x, y, w, h, focused ? accent : alpha(kDim, 90));
    if (focused) {
        outline(renderer, x + 2, y + 2, w - 4, h - 4, alpha(accent, 85));
    }
    rect(renderer, x, y, w, 28, focused ? alpha(accent, 230) : kPanel2);
    text(renderer, x + 10, y + 8, fit(title, 22), 1, focused ? kBg : kInk, false, 1);
    micro(renderer, x + w - 10 - text_width(side_hint, 1, 0), y + 9, side_hint, focused ? kBg : kDim);
}

void tile(SDL_Renderer* renderer, int x, int y, int w, int h, const std::string& label,
          bool active, Colour accent, bool hot = false) {
    const Colour base = active ? accent : (hot ? kPanel3 : kPanel2);
    rect(renderer, x, y, w, h, base);
    outline(renderer, x, y, w, h, active ? kWhite : alpha(accent, 125));
    if (active) outline(renderer, x + 2, y + 2, w - 4, h - 4, alpha(kBg, 100));
    text(renderer, x + w / 2, y + h / 2 - 4, fit(label, 10), 1, active ? kBg : kInk, true, 0);
}

void draw_colour_panel(SDL_Renderer* renderer, int x, int y, int w, const UiState& ui,
                       const InputState& input) {
    const Colour accent = left_accent(ui.left_mode);
    const Direction shown = performance_direction(input, ui);
    constexpr int tw = 58;
    constexpr int th = 38;
    constexpr int gap = 7;
    const int gx = x + (w - (tw * 3 + gap * 2)) / 2;
    const int gy = y + 48;
    const std::array<std::pair<Direction, std::pair<int,int>>, 9> cells{{
        {Direction::UpLeft, {0,0}}, {Direction::Up, {1,0}}, {Direction::UpRight, {2,0}},
        {Direction::Left, {0,1}}, {Direction::Center, {1,1}}, {Direction::Right, {2,1}},
        {Direction::DownLeft, {0,2}}, {Direction::Down, {1,2}}, {Direction::DownRight, {2,2}}
    }};
    for (const auto& [direction, cell] : cells) {
        const std::string label = direction == Direction::Center ? "BASE" : direction_label(ui.palette, direction);
        tile(renderer, gx + cell.first * (tw + gap), gy + cell.second * (th + gap), tw, th,
             label, shown == direction, accent);
    }
    micro(renderer, x + 14, y + 197, "DPAD ARMS A COLOUR", kDim);
    micro(renderer, x + 14, y + 211, "SAME DIRECTION = BASE", kDim);
    micro(renderer, x + 14, y + 232, "HOLD DPAD WITH CHORD", kDim);
    micro(renderer, x + 14, y + 246, "FOR TEMPORARY COLOUR", kDim);
}

std::string percent(float value) {
    return std::to_string(static_cast<int>(std::round(std::clamp(value, 0.0F, 1.0F) * 100.0F))) + "%";
}

void parameter_row(SDL_Renderer* renderer, int x, int y, int w, const std::string& name,
                   const std::string& value, bool active, Colour accent, float meter = -1.0F) {
    rect(renderer, x, y, w, 25, active ? alpha(accent, 235) : kPanel2);
    if (!active) outline(renderer, x, y, w, 25, alpha(accent, 65));
    micro(renderer, x + 8, y + 7, fit(name, 18), active ? kBg : kInk);
    const std::string shown = fit(value, 13);
    micro(renderer, x + w - 8 - text_width(shown, 1, 0), y + 7, shown, active ? kBg : kDim);
    if (meter >= 0.0F) bar(renderer, x + w - 82, y + 18, 72, meter, active ? kBg : accent);
}

void draw_sound_panel(SDL_Renderer* renderer, int x, int y, int w, const PerformanceState& state,
                      const UiState& ui, const SynthEngine& synth) {
    const auto info = synth.preset_info();
    const std::array<std::string, 9> names{
        "ENGINE", "TONE", "BODY", "MOTION", "ATTACK", "RELEASE", "SPREAD", "BPM", "PLAY MODE"
    };
    const std::array<std::string, 9> values{
        info.name,
        percent(synth.parameter(SynthParameter::Tone)),
        percent(synth.parameter(SynthParameter::Body)),
        percent(synth.parameter(SynthParameter::Motion)),
        percent(synth.parameter(SynthParameter::Attack)),
        percent(synth.parameter(SynthParameter::Release)),
        percent(synth.parameter(SynthParameter::Spread)),
        std::to_string(state.bpm),
        mode_name(state.mode)
    };
    for (int row = 0; row < 9; ++row) {
        float meter = -1.0F;
        if (row >= 1 && row <= 6) meter = synth.parameter(static_cast<SynthParameter>(row - 1));
        parameter_row(renderer, x + 10, y + 38 + row * 26, w - 20, names[static_cast<std::size_t>(row)],
                      values[static_cast<std::size_t>(row)], row == ui.sound_row, kOrange, meter);
    }
}

void draw_system_panel(SDL_Renderer* renderer, int x, int y, int w, const PerformanceState& state,
                       const UiState& ui) {
    const std::array<std::string, 4> names{"KEY", "OCTAVE", "UI MOTION", "VERSION"};
    const std::array<std::string, 4> values{
        note_name(state.key_pc),
        (state.octave >= 0 ? "+" : "") + std::to_string(state.octave) + "  FRONT",
        ui.ui_motion == 0 ? "OFF" : ui.ui_motion == 1 ? "LOW" : "FULL",
        std::string("V") + kVersion
    };
    for (int row = 0; row < 4; ++row) {
        parameter_row(renderer, x + 10, y + 48 + row * 38, w - 20, names[static_cast<std::size_t>(row)],
                      values[static_cast<std::size_t>(row)], row == ui.system_row, kBlue);
    }
    micro(renderer, x + 14, y + 216, "FRONT LEFT  : OCTAVE -", kDim);
    micro(renderer, x + 14, y + 230, "FRONT RIGHT : OCTAVE +", kDim);
    micro(renderer, x + 14, y + 250, "HOLD SELECT : ALL NOTES OFF", kDim);
}

struct FaceVisual {
    Face face;
    const char* physical_label;
    int index;
    int x;
    int y;
    Colour colour;
};

void chord_button(SDL_Renderer* renderer, const FaceVisual& visual, int panel_x, int panel_y,
                  const PerformanceState& state, const UiState& ui, const InputState& input,
                  float pulse) {
    const int bx = panel_x + visual.x;
    const int by = panel_y + visual.y;
    constexpr int bw = 82;
    constexpr int bh = 52;
    const bool active = input.face[static_cast<std::size_t>(visual.index)];
    if (active) {
        const int grow = 2 + static_cast<int>(pulse * 3.0F);
        outline(renderer, bx - grow, by - grow, bw + grow * 2, bh + grow * 2, alpha(visual.colour, 110));
        outline(renderer, bx - grow - 2, by - grow - 2, bw + grow * 2 + 4, bh + grow * 2 + 4,
                alpha(visual.colour, 55));
    }
    rect(renderer, bx, by, bw, bh, active ? visual.colour : kPanel2);
    outline(renderer, bx, by, bw, bh, active ? kWhite : alpha(visual.colour, 150));
    text(renderer, bx + 8, by + 7, visual.physical_label, 2, active ? kBg : visual.colour, false, 1);
    const std::string label = face_label(state.key_pc, visual.face, ui.bank);
    micro(renderer, bx + bw / 2, by + 35, fit(label, 15), active ? kBg : kInk, true);
}

void draw_chord_panel(SDL_Renderer* renderer, int x, int y, const PerformanceState& state,
                      const UiState& ui, const InputState& input, const SynthEngine& synth) {
    const float pulse = 0.5F + 0.5F * std::sin(static_cast<float>(SDL_GetTicks()) * 0.012F);
    const std::array<FaceVisual, 4> buttons{{
        {Face::Y, "X", 3, 78, 42, kGreen},
        {Face::X, "Y", 2, 16, 102, kBlue},
        {Face::B, "A", 1, 140, 102, kRed},
        {Face::A, "B", 0, 78, 162, kOrange}
    }};
    for (const auto& button : buttons) chord_button(renderer, button, x, y, state, ui, input, pulse);
    const auto info = synth.preset_info();
    micro(renderer, x + 119, y + 226, fit(info.name, 24), kInk, true);
    micro(renderer, x + 119, y + 241, fit(bank_name(ui.bank), 22), right_accent(ui.right_mode), true);
}

void draw_fx_panel(SDL_Renderer* renderer, int x, int y, int w, const UiState& ui,
                   const SynthEngine& synth, int slot) {
    const auto fx = synth.effect(slot);
    const int index = slot == 0 ? 0 : 1;
    const int selected = ui.fx_row[static_cast<std::size_t>(index)];
    const std::array<std::string, 3> names{"TYPE", "AMOUNT", "COLOUR"};
    const std::array<std::string, 3> values{
        SynthEngine::effect_name(fx.type), percent(fx.amount), percent(fx.colour)
    };
    const Colour accent = slot == 0 ? kRed : kOrange;
    for (int row = 0; row < 3; ++row) {
        const float meter = row == 1 ? fx.amount : row == 2 ? fx.colour : -1.0F;
        parameter_row(renderer, x + 10, y + 46 + row * 38, w - 20, names[static_cast<std::size_t>(row)],
                      values[static_cast<std::size_t>(row)], row == selected, accent, meter);
    }
    const float time = static_cast<float>(SDL_GetTicks()) * 0.006F;
    const int mid = y + 220;
    for (int px = x + 16; px < x + w - 16; ++px) {
        const float local = static_cast<float>(px - x) * 0.08F;
        const int py = mid + static_cast<int>(std::sin(local + time) * (8.0F + fx.amount * 20.0F)
                                             + std::sin(local * 0.37F - time * 0.6F) * fx.colour * 8.0F);
        if (px > x + 16) line(renderer, px - 1, mid, px, py, alpha(accent, 130));
    }
    micro(renderer, x + 14, y + 250, "DPAD UP/DN : SELECT", kDim);
    micro(renderer, x + 14, y + 264, "DPAD LT/RT : EDIT", kDim);
}

void draw_master_panel(SDL_Renderer* renderer, int x, int y, int w, const SynthEngine& synth,
                       const std::optional<ChordSpec>& current) {
    const float master = synth.parameter(SynthParameter::Master);
    parameter_row(renderer, x + 10, y + 48, w - 20, "MASTER LEVEL", percent(master), true, kTeal, master);
    const auto one = synth.effect(0);
    const auto two = synth.effect(1);
    micro(renderer, x + 16, y + 100, "SIGNAL PATH", kDim);
    tile(renderer, x + 18, y + 120, w - 36, 34, "SYNTH", false, kPurple);
    text(renderer, x + w / 2, y + 160, "V", 1, kDim, true, 1);
    tile(renderer, x + 18, y + 176, w - 36, 34, "FX1 " + fit(SynthEngine::effect_name(one.type), 13), false, kRed);
    text(renderer, x + w / 2, y + 216, "V", 1, kDim, true, 1);
    tile(renderer, x + 18, y + 232, w - 36, 34, "FX2 " + fit(SynthEngine::effect_name(two.type), 13), false, kOrange);
    if (current) {
        micro(renderer, x + w / 2, y + 273, fit(current->degree + " " + current->name, 28), kInk, true);
    }
}

void draw_footer(SDL_Renderer* renderer, const PerformanceState& state, const UiState& ui,
                 const InputState& input, const std::optional<ChordSpec>& current,
                 const SynthEngine& synth) {
    rect(renderer, 12, 326, 488, 46, alpha(kPanel, 248));
    outline(renderer, 12, 326, 488, 46, alpha(kDim, 75));
    if (current) {
        text(renderer, 24, 335, fit(current->degree + " " + current->name, 18), 2, kOrange);
        micro(renderer, 24, 357, fit(current->modifier, 23), kInk);
    } else {
        text(renderer, 24, 335, "READY", 2, kInk);
        micro(renderer, 24, 357, "ABXY PLAY FROM EVERY MODE", kDim);
    }
    micro(renderer, 238, 335, "FRONT: OCT - / +", kDim);
    micro(renderer, 238, 349, "REAR L/R: PANEL MODE", kDim);
    micro(renderer, 238, 363, state.latch ? "SELECT: LATCH ON" : "START: MODE  SELECT: LATCH", state.latch ? kGreen : kDim);
    const float peak = synth.output_peak();
    const int segments = 12;
    for (int i = 0; i < segments; ++i) {
        const float threshold = static_cast<float>(i + 1) / static_cast<float>(segments);
        rect(renderer, 460 + (i % 3) * 8, 334 + (i / 3) * 8, 5, 5,
             peak >= threshold ? (threshold > 0.8F ? kRed : kTeal) : kBgLift);
    }
    if (input.front_left || input.front_right) {
        outline(renderer, 232, 331, 132, 18, kBlue);
    }
}

void draw_ui(SDL_Renderer* renderer, const PerformanceState& state, const UiState& ui,
             const InputState& input, const std::optional<ChordSpec>& current,
             const SynthEngine& synth) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    draw_background(renderer, ui, synth.output_peak());
    draw_header(renderer, state, ui, synth);

    constexpr int panel_y = 44;
    constexpr int panel_h = 272;
    constexpr int panel_w = 238;
    constexpr int left_x = 12;
    constexpr int right_x = 262;

    panel_frame(renderer, left_x, panel_y, panel_w, panel_h, ui.focus == Focus::Left,
                left_accent(ui.left_mode), left_mode_name(ui.left_mode), "REAR L");
    panel_frame(renderer, right_x, panel_y, panel_w, panel_h, ui.focus == Focus::Right,
                right_accent(ui.right_mode), right_mode_name(ui.right_mode), "REAR R");

    if (colour_mode(ui.left_mode)) draw_colour_panel(renderer, left_x, panel_y, panel_w, ui, input);
    else if (ui.left_mode == LeftMode::Sound) draw_sound_panel(renderer, left_x, panel_y, panel_w, state, ui, synth);
    else draw_system_panel(renderer, left_x, panel_y, panel_w, state, ui);

    if (chord_mode(ui.right_mode)) draw_chord_panel(renderer, right_x, panel_y, state, ui, input, synth);
    else if (ui.right_mode == RightMode::Fx1) draw_fx_panel(renderer, right_x, panel_y, panel_w, ui, synth, 0);
    else if (ui.right_mode == RightMode::Fx2) draw_fx_panel(renderer, right_x, panel_y, panel_w, ui, synth, 1);
    else draw_master_panel(renderer, right_x, panel_y, panel_w, synth, current);

    draw_footer(renderer, state, ui, input, current, synth);

    if (!ui.toast.empty() && SDL_GetTicks() < ui.toast_until) {
        const int w = std::clamp(text_width(ui.toast, 2, 1) + 28, 120, 400);
        const int x = (kWidth - w) / 2;
        rect(renderer, x, 169, w, 40, alpha(kBg, 245));
        outline(renderer, x, 169, w, 40, ui.focus == Focus::Left ? left_accent(ui.left_mode) : right_accent(ui.right_mode));
        text(renderer, kWidth / 2, 182, fit(ui.toast, 31), 2,
             ui.focus == Focus::Left ? left_accent(ui.left_mode) : right_accent(ui.right_mode), true, 1);
    }
}

std::optional<Face> face_button(SDL_GameControllerButton button) {
    // Knulli maps by position using the SDL/Xbox convention, while the Brick cap labels
    // follow the Nintendo-style layout. Keep the musical positions and show the real cap labels.
    switch (button) {
    case SDL_CONTROLLER_BUTTON_A: return Face::A; // physical B, bottom
    case SDL_CONTROLLER_BUTTON_B: return Face::B; // physical A, right
    case SDL_CONTROLLER_BUTTON_X: return Face::X; // physical Y, left
    case SDL_CONTROLLER_BUTTON_Y: return Face::Y; // physical X, top
    default: return std::nullopt;
    }
}

void set_dpad(InputState& input, SDL_GameControllerButton button, bool down) {
    if (button == SDL_CONTROLLER_BUTTON_DPAD_UP) input.up = down;
    if (button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) input.down = down;
    if (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) input.left = down;
    if (button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) input.right = down;
}

void update_chord(PerformanceState& state, UiState& ui, InputState& input,
                  std::optional<ChordSpec>& current, std::vector<int>& previous,
                  SynthEngine& synth) {
    if (!input.active_face) return;
    state.chord_bank = ui.bank;
    state.colour_palette = ui.palette;
    const auto chord = make_chord(state.key_pc, state.octave, *input.active_face,
                                  state.chord_bank, state.colour_palette,
                                  performance_direction(input, ui));
    const auto notes = voice_lead(chord, previous);
    previous = notes;
    current = chord;
    synth.play_chord(notes);
}

void release_face(InputState& input, Face face, std::optional<ChordSpec>& current, SynthEngine& synth) {
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

void cycle_left(UiState& ui) {
    const Uint32 now = SDL_GetTicks();
    if (now - ui.last_left_cycle < 130U) return;
    ui.last_left_cycle = now;
    const int count = static_cast<int>(LeftMode::Count);
    ui.left_mode = static_cast<LeftMode>((static_cast<int>(ui.left_mode) + 1) % count);
    ui.focus = Focus::Left;
    ui.palette = palette_for(ui.left_mode, ui.palette);
    toast(ui, "LEFT: " + left_mode_name(ui.left_mode));
}

void cycle_right(UiState& ui) {
    const Uint32 now = SDL_GetTicks();
    if (now - ui.last_right_cycle < 130U) return;
    ui.last_right_cycle = now;
    const int count = static_cast<int>(RightMode::Count);
    ui.right_mode = static_cast<RightMode>((static_cast<int>(ui.right_mode) + 1) % count);
    ui.focus = Focus::Right;
    ui.bank = bank_for(ui.right_mode, ui.bank);
    toast(ui, "RIGHT: " + right_mode_name(ui.right_mode));
}

void change_octave(PerformanceState& state, UiState& ui, int delta,
                   InputState& input, std::optional<ChordSpec>& current,
                   std::vector<int>& previous, SynthEngine& synth) {
    state.octave = std::clamp(state.octave + delta, -2, 2);
    toast(ui, "OCTAVE " + std::string(state.octave >= 0 ? "+" : "") + std::to_string(state.octave));
    if (input.active_face) update_chord(state, ui, input, current, previous, synth);
}

void edit_sound(UiState& ui, PerformanceState& state, SynthEngine& synth, int delta) {
    const int row = ui.sound_row;
    if (row == 0) {
        state.preset = (state.preset + delta + SynthEngine::preset_count()) % SynthEngine::preset_count();
        synth.set_preset(state.preset);
        toast(ui, synth.preset_info().name);
    } else if (row >= 1 && row <= 6) {
        const auto parameter = static_cast<SynthParameter>(row - 1);
        synth.set_parameter(parameter, synth.parameter(parameter) + static_cast<float>(delta) * 0.02F);
    } else if (row == 7) {
        state.bpm = std::clamp(state.bpm + delta, 40, 240);
        synth.set_bpm(state.bpm);
    } else if (row == 8) {
        const int mode = (static_cast<int>(state.mode) + delta + 4) % 4;
        state.mode = static_cast<PlayMode>(mode);
        synth.set_mode(state.mode);
    }
}

void edit_system(UiState& ui, PerformanceState& state, int delta) {
    if (ui.system_row == 0) state.key_pc = (state.key_pc + delta + 12) % 12;
    else if (ui.system_row == 2) ui.ui_motion = std::clamp(ui.ui_motion + delta, 0, 2);
}

void edit_fx(UiState& ui, SynthEngine& synth, int slot, int delta) {
    auto fx = synth.effect(slot);
    const int row = ui.fx_row[static_cast<std::size_t>(slot)];
    if (row == 0) {
        const int type = (static_cast<int>(fx.type) + delta + SynthEngine::effect_count()) % SynthEngine::effect_count();
        fx.type = static_cast<EffectType>(type);
    } else if (row == 1) {
        fx.amount += static_cast<float>(delta) * 0.02F;
    } else {
        fx.colour += static_cast<float>(delta) * 0.02F;
    }
    synth.set_effect(slot, fx);
}

void edit_master(SynthEngine& synth, int delta) {
    synth.set_parameter(SynthParameter::Master,
                        synth.parameter(SynthParameter::Master) + static_cast<float>(delta) * 0.02F);
}

void handle_dpad_press(UiState& ui, PerformanceState& state, InputState& input,
                       SynthEngine& synth, SDL_GameControllerButton button) {
    const bool up = button == SDL_CONTROLLER_BUTTON_DPAD_UP;
    const bool down = button == SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    const bool left = button == SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    const bool right = button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT;

    if (ui.focus == Focus::Left && colour_mode(ui.left_mode)) {
        const Direction direction = held_direction(input);
        if (direction != Direction::Center) {
            ui.armed = ui.armed == direction ? Direction::Center : direction;
            toast(ui, ui.armed == Direction::Center ? "BASE" : direction_label(ui.palette, ui.armed));
        }
        return;
    }

    if (ui.focus == Focus::Left && ui.left_mode == LeftMode::Sound) {
        if (up) ui.sound_row = (ui.sound_row + 8) % 9;
        if (down) ui.sound_row = (ui.sound_row + 1) % 9;
        if (left || right) edit_sound(ui, state, synth, right ? 1 : -1);
    } else if (ui.focus == Focus::Left && ui.left_mode == LeftMode::System) {
        if (up) ui.system_row = (ui.system_row + 3) % 4;
        if (down) ui.system_row = (ui.system_row + 1) % 4;
        if (left || right) edit_system(ui, state, right ? 1 : -1);
    } else if (ui.focus == Focus::Right && ui.right_mode == RightMode::Fx1) {
        int& row = ui.fx_row[0];
        if (up) row = (row + 2) % 3;
        if (down) row = (row + 1) % 3;
        if (left || right) edit_fx(ui, synth, 0, right ? 1 : -1);
    } else if (ui.focus == Focus::Right && ui.right_mode == RightMode::Fx2) {
        int& row = ui.fx_row[1];
        if (up) row = (row + 2) % 3;
        if (down) row = (row + 1) % 3;
        if (left || right) edit_fx(ui, synth, 1, right ? 1 : -1);
    } else if (ui.focus == Focus::Right && ui.right_mode == RightMode::Master) {
        if (left || right) edit_master(synth, right ? 1 : -1);
    } else {
        // Chord bank view: D-pad remains a performance colour controller.
        const Direction direction = held_direction(input);
        if (direction != Direction::Center) {
            ui.armed = ui.armed == direction ? Direction::Center : direction;
            toast(ui, ui.armed == Direction::Center ? "BASE" : direction_label(ui.palette, ui.armed));
        }
    }

    if (left || right) {
        ui.held_edit = right ? 1 : -1;
        ui.held_since = SDL_GetTicks();
        ui.last_repeat = ui.held_since;
    }
}

void repeat_edit(UiState& ui, PerformanceState& state, SynthEngine& synth) {
    if (ui.held_edit == 0) return;
    const Uint32 now = SDL_GetTicks();
    const Uint32 age = now - ui.held_since;
    const Uint32 interval = age > 2200U ? 40U : age > 850U ? 90U : 100000U;
    if (now - ui.last_repeat < interval) return;
    if (ui.focus == Focus::Left && ui.left_mode == LeftMode::Sound) edit_sound(ui, state, synth, ui.held_edit);
    else if (ui.focus == Focus::Left && ui.left_mode == LeftMode::System) edit_system(ui, state, ui.held_edit);
    else if (ui.focus == Focus::Right && ui.right_mode == RightMode::Fx1) edit_fx(ui, synth, 0, ui.held_edit);
    else if (ui.focus == Focus::Right && ui.right_mode == RightMode::Fx2) edit_fx(ui, synth, 1, ui.held_edit);
    else if (ui.focus == Focus::Right && ui.right_mode == RightMode::Master) edit_master(synth, ui.held_edit);
    ui.last_repeat = now;
}

void log_controller(SDL_GameController* controller) {
    if (!controller) return;
    SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);
    std::cerr << "controller: " << SDL_GameControllerName(controller)
              << " buttons=" << SDL_JoystickNumButtons(joystick)
              << " axes=" << SDL_JoystickNumAxes(joystick)
              << " hats=" << SDL_JoystickNumHats(joystick) << '\n';
    if (char* mapping = SDL_GameControllerMapping(controller)) {
        std::cerr << "mapping: " << mapping << '\n';
        SDL_free(mapping);
    }
}

} // namespace

int main(int, char**) {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER |
                 SDL_INIT_JOYSTICK | SDL_INIT_EVENTS) != 0) {
        std::cerr << "SDL init: " << SDL_GetError() << '\n';
        return 1;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_Window* window = SDL_CreateWindow("BRKCHRD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "window: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_RenderSetLogicalSize(renderer, kWidth, kHeight);

    SynthEngine synth(kRate);
    PerformanceState state;
    UiState ui;
    load_config(state, ui, synth);
    synth.set_mode(state.mode);
    synth.set_bpm(state.bpm);
    synth.set_latch(state.latch);

    SDL_AudioSpec desired{};
    desired.freq = kRate;
    desired.format = AUDIO_F32SYS;
    desired.channels = 2;
    desired.samples = kFrames;
    desired.callback = audio_callback;
    desired.userdata = &synth;
    SDL_AudioSpec obtained{};
    const SDL_AudioDeviceID audio = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (audio != 0) SDL_PauseAudioDevice(audio, 0);
    else std::cerr << "audio: " << SDL_GetError() << '\n';

    SDL_GameController* controller = nullptr;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            if (controller) break;
        }
    }
    log_controller(controller);

    InputState input;
    std::optional<ChordSpec> current;
    std::vector<int> previous;
    bool running = true;
    int raw_log_budget = 64;
    toast(ui, "BRKCHRD 0.3  BRICK LAYOUT", 1400U);

    while (running) {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_CONTROLLERDEVICEADDED && !controller) {
                controller = SDL_GameControllerOpen(event.cdevice.which);
                log_controller(controller);
            }
            if (event.type == SDL_CONTROLLERDEVICEREMOVED && controller &&
                SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller)) == event.cdevice.which) {
                SDL_GameControllerClose(controller);
                controller = nullptr;
            }

            if ((event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYAXISMOTION) && raw_log_budget > 0) {
                if (event.type == SDL_JOYBUTTONDOWN) {
                    std::cerr << "raw button down: " << static_cast<int>(event.jbutton.button) << '\n';
                    --raw_log_budget;
                } else if (std::abs(event.jaxis.value) > 15000) {
                    std::cerr << "raw axis: " << static_cast<int>(event.jaxis.axis)
                              << " value=" << event.jaxis.value << '\n';
                    --raw_log_budget;
                }
            }

            if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP) {
                const bool down = event.type == SDL_CONTROLLERBUTTONDOWN;
                const auto button = static_cast<SDL_GameControllerButton>(event.cbutton.button);
                if (down && raw_log_budget > 0) {
                    const char* name = SDL_GameControllerGetStringForButton(button);
                    std::cerr << "controller button down: " << static_cast<int>(button)
                              << " " << (name ? name : "?") << '\n';
                    --raw_log_budget;
                }

                if (auto face = face_button(button)) {
                    if (down) {
                        input.face[static_cast<std::size_t>(*face)] = true;
                        input.active_face = *face;
                        update_chord(state, ui, input, current, previous, synth);
                    } else {
                        release_face(input, *face, current, synth);
                        if (input.active_face) update_chord(state, ui, input, current, previous, synth);
                    }
                } else if (button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) {
                    input.front_left = down;
                    if (down) change_octave(state, ui, -1, input, current, previous, synth);
                } else if (button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) {
                    input.front_right = down;
                    if (down) change_octave(state, ui, 1, input, current, previous, synth);
                } else if (button == SDL_CONTROLLER_BUTTON_LEFTSTICK) {
                    input.rear_left = down;
                    if (down) cycle_left(ui);
                } else if (button == SDL_CONTROLLER_BUTTON_RIGHTSTICK) {
                    input.rear_right = down;
                    if (down) cycle_right(ui);
                } else if (button == SDL_CONTROLLER_BUTTON_DPAD_UP ||
                           button == SDL_CONTROLLER_BUTTON_DPAD_DOWN ||
                           button == SDL_CONTROLLER_BUTTON_DPAD_LEFT ||
                           button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
                    set_dpad(input, button, down);
                    if (down) {
                        if (input.active_face) update_chord(state, ui, input, current, previous, synth);
                        else handle_dpad_press(ui, state, input, synth, button);
                    } else {
                        if (input.active_face) update_chord(state, ui, input, current, previous, synth);
                        if (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT ||
                            button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) ui.held_edit = 0;
                    }
                } else if (button == SDL_CONTROLLER_BUTTON_START) {
                    input.start = down;
                    if (!down && !input.select) {
                        state.mode = static_cast<PlayMode>((static_cast<int>(state.mode) + 1) % 4);
                        synth.set_mode(state.mode);
                        toast(ui, mode_name(state.mode));
                    }
                } else if (button == SDL_CONTROLLER_BUTTON_BACK) {
                    if (down) {
                        input.select = true;
                        input.select_long = false;
                        input.select_since = SDL_GetTicks();
                    } else {
                        if (!input.select_long && !input.start) {
                            state.latch = !state.latch;
                            synth.set_latch(state.latch);
                            toast(ui, state.latch ? "LATCH ON" : "LATCH OFF");
                        }
                        input.select = false;
                    }
                }
                if (input.start && input.select) running = false;
            }

            if (event.type == SDL_CONTROLLERAXISMOTION) {
                const bool down = event.caxis.value > 15000;
                if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT && input.rear_left != down) {
                    input.rear_left = down;
                    if (down) cycle_left(ui);
                }
                if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT && input.rear_right != down) {
                    input.rear_right = down;
                    if (down) cycle_right(ui);
                }
            }

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                const bool down = event.type == SDL_KEYDOWN;
                const SDL_Keycode key = event.key.keysym.sym;
                auto keyboard_face = [&](Face face) {
                    if (down) {
                        input.face[static_cast<std::size_t>(face)] = true;
                        input.active_face = face;
                        update_chord(state, ui, input, current, previous, synth);
                    } else {
                        release_face(input, face, current, synth);
                    }
                };
                if (key == SDLK_z) keyboard_face(Face::A);
                else if (key == SDLK_x) keyboard_face(Face::B);
                else if (key == SDLK_a) keyboard_face(Face::X);
                else if (key == SDLK_s) keyboard_face(Face::Y);
                else if (key == SDLK_q && down && event.key.repeat == 0) cycle_left(ui);
                else if (key == SDLK_w && down && event.key.repeat == 0) cycle_right(ui);
                else if (key == SDLK_e && down && event.key.repeat == 0) change_octave(state, ui, -1, input, current, previous, synth);
                else if (key == SDLK_r && down && event.key.repeat == 0) change_octave(state, ui, 1, input, current, previous, synth);
                else if (key == SDLK_UP || key == SDLK_DOWN || key == SDLK_LEFT || key == SDLK_RIGHT) {
                    const SDL_GameControllerButton button =
                        key == SDLK_UP ? SDL_CONTROLLER_BUTTON_DPAD_UP :
                        key == SDLK_DOWN ? SDL_CONTROLLER_BUTTON_DPAD_DOWN :
                        key == SDLK_LEFT ? SDL_CONTROLLER_BUTTON_DPAD_LEFT :
                                          SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
                    set_dpad(input, button, down);
                    if (down) {
                        if (input.active_face) update_chord(state, ui, input, current, previous, synth);
                        else handle_dpad_press(ui, state, input, synth, button);
                    } else {
                        if (input.active_face) update_chord(state, ui, input, current, previous, synth);
                        if (key == SDLK_LEFT || key == SDLK_RIGHT) ui.held_edit = 0;
                    }
                } else if (key == SDLK_RETURN && down && event.key.repeat == 0) {
                    state.mode = static_cast<PlayMode>((static_cast<int>(state.mode) + 1) % 4);
                    synth.set_mode(state.mode);
                    toast(ui, mode_name(state.mode));
                } else if (key == SDLK_SPACE && down && event.key.repeat == 0) {
                    state.latch = !state.latch;
                    synth.set_latch(state.latch);
                    toast(ui, state.latch ? "LATCH ON" : "LATCH OFF");
                } else if (key == SDLK_ESCAPE && down) {
                    running = false;
                }
            }
        }

        if (input.select && !input.start && !input.select_long &&
            SDL_GetTicks() - input.select_since > 850U) {
            input.select_long = true;
            state.latch = false;
            synth.set_latch(false);
            synth.all_notes_off();
            current.reset();
            input.active_face.reset();
            input.face.fill(false);
            toast(ui, "ALL NOTES OFF", 1200U);
        }

        repeat_edit(ui, state, synth);
        if (SDL_GetTicks() >= ui.toast_until) ui.toast.clear();
        draw_ui(renderer, state, ui, input, current, synth);
        SDL_RenderPresent(renderer);
        SDL_Delay(8);
    }

    save_config(state, ui, synth);
    synth.all_notes_off();
    if (audio != 0) SDL_CloseAudioDevice(audio);
    if (controller) SDL_GameControllerClose(controller);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
