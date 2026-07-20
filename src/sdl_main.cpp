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
constexpr const char* kVersion = "0.4.0";

struct Colour { Uint8 r, g, b, a = 255; };
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

void colour(SDL_Renderer* r, Colour c) { SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a); }
void rect(SDL_Renderer* r, int x, int y, int w, int h, Colour c) {
    colour(r, c); SDL_Rect q{x, y, w, h}; SDL_RenderFillRect(r, &q);
}
void outline(SDL_Renderer* r, int x, int y, int w, int h, Colour c) {
    colour(r, c); SDL_Rect q{x, y, w, h}; SDL_RenderDrawRect(r, &q);
}
void line(SDL_Renderer* r, int x1, int y1, int x2, int y2, Colour c) {
    colour(r, c); SDL_RenderDrawLine(r, x1, y1, x2, y2);
}
Colour alpha(Colour c, Uint8 a) { c.a = a; return c; }

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
    return value.empty() ? 0 : static_cast<int>(value.size()) * (5 * scale + tracking) - tracking;
}
std::string fit(std::string value, int chars) {
    if (chars < 1) return {};
    if (static_cast<int>(value.size()) <= chars) return value;
    if (chars <= 3) return value.substr(0, static_cast<std::size_t>(chars));
    return value.substr(0, static_cast<std::size_t>(chars - 3)) + "...";
}
void text(SDL_Renderer* r, int x, int y, std::string_view value, int scale, Colour c,
          bool centered = false, int tracking = 1) {
    if (centered) x -= text_width(value, scale, tracking) / 2;
    colour(r, c);
    int cursor = x;
    for (char ch : value) {
        const auto rows = glyph(ch);
        for (int row = 0; row < 7; ++row) for (int col = 0; col < 5; ++col) {
            if (((rows[static_cast<std::size_t>(row)] >> (4 - col)) & 1U) != 0U) {
                SDL_Rect px{cursor + col * scale, y + row * scale, scale, scale};
                SDL_RenderFillRect(r, &px);
            }
        }
        cursor += 5 * scale + tracking;
    }
}
void micro(SDL_Renderer* r, int x, int y, std::string_view value, Colour c, bool centered = false) {
    text(r, x, y, value, 1, c, centered, 0);
}
void bar(SDL_Renderer* r, int x, int y, int w, float value, Colour active) {
    rect(r, x, y, w, 5, kBg); outline(r, x, y, w, 5, alpha(kDim, 150));
    rect(r, x + 1, y + 1, static_cast<int>(static_cast<float>(w - 2) * std::clamp(value, 0.0F, 1.0F)), 3, active);
}

enum class DpadMode { Chord = 0, Sound, Fx, Count };

struct InputState {
    std::array<bool, 4> face{};
    bool up = false, down = false, left = false, right = false;
    bool front_left = false, front_right = false;
    bool l1 = false, l2 = false, r1 = false, r2 = false;
    bool start = false, select = false;
    bool start_long = false;
    Uint32 start_since = 0U;
    std::optional<Face> active_face;
};

struct UiState {
    DpadMode dpad_mode = DpadMode::Chord;
    bool settings = false;
    Direction selected_direction = Direction::Center;
    int selected_palette = 0;
    int default_palette = 0;
    int l1_palette = 2;
    int default_bank = 0;
    int r1_bank = 1;
    int r2_bank = 2;
    std::array<int, 2> sound_row{};
    int settings_row = 0;
    int settings_scroll = 0;
    int ui_motion = 2;
    bool swap_left_rear = false;
    bool swap_right_rear = false;
    std::string toast;
    Uint32 toast_until = 0U;
    int held_edit = 0;
    Uint32 held_since = 0U;
    Uint32 last_repeat = 0U;
    bool perf_fx_active = false;
    Direction perf_fx_direction = Direction::Center;
    int perf_fx_layer = 0;
    std::array<EffectSettings, 2> fx_restore{};
};

std::filesystem::path config_path() {
    if (const char* root = std::getenv("XDG_CONFIG_HOME")) return std::filesystem::path(root) / "brkchrd.cfg";
    if (const char* home = std::getenv("HOME")) return std::filesystem::path(home) / ".config" / "brkchrd.cfg";
    return "brkchrd.cfg";
}
void toast(UiState& ui, std::string value, Uint32 ms = 1000U) {
    ui.toast = std::move(value); ui.toast_until = SDL_GetTicks() + ms;
}
Direction held_direction(const InputState& in) {
    if (in.up && in.left) return Direction::UpLeft;
    if (in.up && in.right) return Direction::UpRight;
    if (in.down && in.left) return Direction::DownLeft;
    if (in.down && in.right) return Direction::DownRight;
    if (in.up) return Direction::Up;
    if (in.right) return Direction::Right;
    if (in.down) return Direction::Down;
    if (in.left) return Direction::Left;
    return Direction::Center;
}
int display_palette(const InputState& in, const UiState& ui) { return in.l1 ? ui.l1_palette : ui.default_palette; }
int active_bank(const InputState& in, const UiState& ui) { return in.r2 ? ui.r2_bank : in.r1 ? ui.r1_bank : ui.default_bank; }
std::string dpad_mode_name(DpadMode mode) {
    switch (mode) { case DpadMode::Chord: return "CHORD"; case DpadMode::Sound: return "SOUND"; case DpadMode::Fx: return "PERF FX"; case DpadMode::Count: break; }
    return "CHORD";
}
std::string ui_motion_name(int value) { return value == 0 ? "OFF" : value == 1 ? "LOW" : "FULL"; }
std::string on_off(bool value)  { return value ? "ON" : "OFF"; }
std::string percent(float value) { return std::to_string(static_cast<int>(std::round(std::clamp(value, 0.0F, 1.0F) * 100.0F))) + "%"; }

void load_config(PerformanceState& p, UiState& ui, SynthEngine& synth) {
    std::ifstream in(config_path());
    std::string key; float value = 0.0F;
    std::array<float, static_cast<std::size_t>(SynthParameter::Count)> params{};
    std::array<bool, static_cast<std::size_t>(SynthParameter::Count)> param_seen{};
    std::array<EffectSettings, 2> effects{}; std::array<bool, 2> effect_seen{};
    while (in >> key >> value) {
        if (key == "key") p.key_pc = std::clamp(static_cast<int>(value), 0, 11);
        else if (key == "octave") p.octave = std::clamp(static_cast<int>(value), -2, 2);
        else if (key == "preset") p.preset = static_cast<int>(value);
        else if (key == "bpm") p.bpm = std::clamp(static_cast<int>(value), 40, 240);
        else if (key == "mode") p.mode = static_cast<PlayMode>(std::clamp(static_cast<int>(value), 0, 3));
        else if (key == "palette" || key == "defaultpalette") ui.default_palette = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "l1palette") ui.l1_palette = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "selectedpalette") ui.selected_palette = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "direction") ui.selected_direction = static_cast<Direction>(std::clamp(static_cast<int>(value), 0, 8));
        else if (key == "bank" || key == "defaultbank") ui.default_bank = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "r1bank") ui.r1_bank = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "r2bank") ui.r2_bank = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "dpadmode") ui.dpad_mode = static_cast<DpadMode>(std::clamp(static_cast<int>(value), 0, 2));
        else if (key == "uimotion") ui.ui_motion = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "swapleft") ui.swap_left_rear = value != 0.0F;
        else if (key == "swapright") ui.swap_right_rear = value != 0.0F;
        else if (key.rfind("param", 0) == 0) {
            const int index = std::clamp(std::stoi(key.substr(5)), 0, static_cast<int>(SynthParameter::Count) - 1);
            params[static_cast<std::size_t>(index)] = value; param_seen[static_cast<std::size_t>(index)] = true;
        } else if (key == "fx1type") { effects[0].type = static_cast<EffectType>(std::clamp(static_cast<int>(value), 0, SynthEngine::effect_count() - 1)); effect_seen[0] = true; }
        else if (key == "fx1amount") { effects[0].amount = value; effect_seen[0] = true; }
        else if (key == "fx1colour") { effects[0].colour = value; effect_seen[0] = true; }
        else if (key == "fx2type") { effects[1].type = static_cast<EffectType>(std::clamp(static_cast<int>(value), 0, SynthEngine::effect_count() - 1)); effect_seen[1] = true; }
        else if (key == "fx2amount") { effects[1].amount = value; effect_seen[1] = true; }
        else if (key == "fx2colour") { effects[1].colour = value; effect_seen[1] = true; }
    }
    p.latch = false;
    synth.set_preset(p.preset); p.preset = synth.preset_index();
    for (int i = 0; i < static_cast<int>(SynthParameter::Count); ++i) if (param_seen[static_cast<std::size_t>(i)]) synth.set_parameter(static_cast<SynthParameter>(i), params[static_cast<std::size_t>(i)]);
    for (int slot = 0; slot < 2; ++slot) if (effect_seen[static_cast<std::size_t>(slot)]) synth.set_effect(slot, effects[static_cast<std::size_t>(slot)]);
}

void save_config(const PerformanceState& p, const UiState& ui, const SynthEngine& synth) {
    const auto path = config_path(); std::error_code ec; std::filesystem::create_directories(path.parent_path(), ec);
    std::ofstream out(path);
    out << "key " << p.key_pc << '\n' << "octave " << p.octave << '\n' << "preset " << synth.preset_index() << '\n'
        << "bpm " << p.bpm << '\n' << "mode " << static_cast<int>(p.mode) << '\n' << "latch 0\n"
        << "defaultpalette " << ui.default_palette << '\n' << "l1palette " << ui.l1_palette << '\n'
        << "selectedpalette " << ui.selected_palette << '\n' << "direction " << static_cast<int>(ui.selected_direction) << '\n'
        << "defaultbank " << ui.default_bank << '\n' << "r1bank " << ui.r1_bank << '\n' << "r2bank " << ui.r2_bank << '\n'
        << "dpadmode " << static_cast<int>(ui.dpad_mode) << '\n' << "uimotion " << ui.ui_motion << '\n'
        << "swapleft " << (ui.swap_left_rear ? 1 : 0) << '\n' << "swapright " << (ui.swap_right_rear ? 1 : 0) << '\n';
    for (int i = 0; i < static_cast<int>(SynthParameter::Count); ++i) out << "param" << i << ' ' << synth.parameter(static_cast<SynthParameter>(i)) << '\n';
    for (int slot = 0; slot < 2; ++slot) {
        const auto fx = synth.effect(slot); const int n = slot + 1;
        out << "fx" << n << "type " << static_cast<int>(fx.type) << '\n'
            << "fx" << n << "amount " << fx.amount << '\n'
            << "fx" << n << "colour " << fx.colour << '\n';
    }
}

void audio_callback(void* userdata, Uint8* bytes, int count) {
    auto* synth = static_cast<SynthEngine*>(userdata);
    synth->render(reinterpret_cast<float*>(bytes), static_cast<std::size_t>(count) / (sizeof(float) * 2U));
}

void draw_background(SDL_Renderer* r, const UiState& ui, float peak) {
    rect(r, 0, 0, kWidth, kHeight, kBg);
    if (ui.ui_motion == 0) return;
    const float t = static_cast<float>(SDL_GetTicks()) * 0.001F;
    const int drift = ui.ui_motion == 2 ? static_cast<int>(std::fmod(t * 8.0F, 24.0F)) : 0;
    for (int x = -24 + drift; x < kWidth + 32; x += 24) line(r, x, 36, x - 56, kHeight, alpha(kPurple, 22));
    for (int y = 46; y < kHeight; y += 24) line(r, 0, y, kWidth, y, alpha(kInk, 12));
    if (ui.ui_motion == 2) {
        const int glow = 22 + static_cast<int>(36.0F * std::clamp(peak, 0.0F, 1.0F));
        rect(r, 0, 34, kWidth, glow, alpha(kPurple, 18));
    }
}

Colour mode_accent(DpadMode mode) {
    if (mode == DpadMode::Sound) return kOrange;
    if (mode == DpadMode::Fx) return kTeal;
    return kPurpleBright;
}

void draw_header(SDL_Renderer* r, const PerformanceState& p, const UiState& ui, float peak) {
    rect(r, 0, 0, kWidth, 36, kPanel);
    text(r, 12, 8, "BRKCHRD", 2, kInk);
    micro(r, 12, 27, std::string("V") + kVersion, kDim);
    const Colour accent = mode_accent(ui.dpad_mode);
    rect(r, 164, 5, 112, 26, accent);
    text(r, 220, 11, dpad_mode_name(ui.dpad_mode), 1, kBg, true, 1);
    micro(r, 292, 9, "OCT", kDim);
    text(r, 323, 8, (p.octave >= 0 ? "+" : "") + std::to_string(p.octave), 2, kInk, true);
    micro(r, 350, 9, "MODE", kDim);
    text(r, 405, 8, mode_name(p.mode), 1, kInk, true);
    rect(r, 462, 9, 38, 12, kBgLift);
    rect(r, 464, 11, static_cast<int>(34.0F * std::clamp(peak, 0.0F, 1.0F)), 8,
         peak > 0.85F ? kRed : peak > 0.55F ? kOrange : kGreen);
    micro(r, 481, 25, "OUT", kDim, true);
}

void panel_frame(SDL_Renderer* r, int x, int y, int w, int h, Colour accent) {
    rect(r, x, y, w, h, kPanel);
    outline(r, x, y, w, h, alpha(accent, 210));
    outline(r, x + 2, y + 2, w - 4, h - 4, alpha(accent, 55));
}

void draw_cell(SDL_Renderer* r, int x, int y, int w, int h, const std::string& label,
               bool active, bool live, Colour accent) {
    Colour fill = active ? accent : live ? kPanel3 : kPanel2;
    rect(r, x, y, w, h, fill);
    outline(r, x, y, w, h, live ? kWhite : alpha(accent, 130));
    text(r, x + w / 2, y + h / 2 - 4, fit(label, 11), 1, active ? kBg : kInk, true, 0);
}

std::array<std::pair<Direction, std::pair<int, int>>, 8> grid_positions(int x, int y, int cw, int ch, int gap) {
    return {{{Direction::UpLeft, {x, y}}, {Direction::Up, {x + cw + gap, y}}, {Direction::UpRight, {x + 2 * (cw + gap), y}},
             {Direction::Left, {x, y + ch + gap}}, {Direction::Right, {x + 2 * (cw + gap), y + ch + gap}},
             {Direction::DownLeft, {x, y + 2 * (ch + gap)}}, {Direction::Down, {x + cw + gap, y + 2 * (ch + gap)}},
             {Direction::DownRight, {x + 2 * (cw + gap), y + 2 * (ch + gap)}}}};
}

void draw_chord_mode(SDL_Renderer* r, const UiState& ui, const InputState& in) {
    constexpr int x = 12, y = 46, w = 238, h = 286;
    const int palette = display_palette(in, ui);
    const Colour accent = palette == 2 ? kRed : palette == 1 ? kPurpleBright : kPurple;
    panel_frame(r, x, y, w, h, accent);
    text(r, x + 12, y + 10, palette_name(palette), 2, accent);
    micro(r, x + w - 12, y + 14, in.l1 ? "L1 HELD" : "L1 ALT", in.l1 ? kWhite : kDim, true);
    micro(r, x + 12, y + 35, "DPAD ARMS CHORD COLOUR", kDim);

    const int gx = x + 10, gy = y + 58, cw = 68, ch = 56, gap = 6;
    const auto positions = grid_positions(gx, gy, cw, ch, gap);
    for (const auto& item : positions) {
        const Direction d = item.first;
        const int px = item.second.first, py = item.second.second;
        const bool active = ui.selected_palette == palette && ui.selected_direction == d;
        draw_cell(r, px, py, cw, ch, direction_label(palette, d), active, false, accent);
    }
    const int cx = gx + cw + gap, cy = gy + ch + gap;
    const bool base = ui.selected_direction == Direction::Center;
    draw_cell(r, cx, cy, cw, ch, "BASE", base, false, accent);

    const std::string selected = ui.selected_direction == Direction::Center
        ? "ACTIVE: BASE"
        : "ACTIVE: " + palette_name(ui.selected_palette) + " / " + direction_label(ui.selected_palette, ui.selected_direction);
    micro(r, x + w / 2, y + h - 18, fit(selected, 43), kInk, true);
}

void parameter_row(SDL_Renderer* r, int x, int y, int w, const std::string& name,
                   const std::string& value, bool active, Colour accent, float amount = -1.0F) {
    rect(r, x, y, w, 36, active ? accent : kPanel2);
    outline(r, x, y, w, 36, active ? kWhite : alpha(accent, 120));
    text(r, x + 9, y + 7, fit(name, 18), 1, active ? kBg : kInk, false, 0);
    text(r, x + w - 9, y + 7, fit(value, 17), 1, active ? kBg : kDim, true, 0);
    if (amount >= 0.0F) bar(r, x + 9, y + 25, w - 18, amount, active ? kBg : accent);
}

void draw_sound_mode(SDL_Renderer* r, const PerformanceState& p, const UiState& ui,
                     const InputState& in, const SynthEngine& synth) {
    constexpr int x = 12, y = 46, w = 238, h = 286;
    const int layer = in.l1 ? 1 : 0;
    panel_frame(r, x, y, w, h, kOrange);
    text(r, x + 12, y + 10, "SOUND", 2, kOrange);
    micro(r, x + w - 12, y + 14, layer == 0 ? "L1: ENV" : "L1 HELD", layer == 1 ? kWhite : kDim, true);
    micro(r, x + 12, y + 35, layer == 0 ? "ENGINE / TONE BANK" : "ENVELOPE / TIME BANK", kDim);

    const int row = ui.sound_row[static_cast<std::size_t>(layer)];
    if (layer == 0) {
        const auto info = synth.preset_info();
        const std::array<std::string, 4> names{"PRESET", "TONE", "BODY", "MOTION"};
        const std::array<std::string, 4> values{info.name, percent(synth.parameter(SynthParameter::Tone)),
            percent(synth.parameter(SynthParameter::Body)), percent(synth.parameter(SynthParameter::Motion))};
        const std::array<float, 4> bars{-1.0F, synth.parameter(SynthParameter::Tone), synth.parameter(SynthParameter::Body), synth.parameter(SynthParameter::Motion)};
        for (int i = 0; i < 4; ++i) parameter_row(r, x + 10, y + 58 + i * 48, w - 20, names[static_cast<std::size_t>(i)], values[static_cast<std::size_t>(i)], i == row, kOrange, bars[static_cast<std::size_t>(i)]);
        micro(r, x + 12, y + h - 19, fit(info.character, 42), kDim);
    } else {
        const std::array<std::string, 5> names{"ATTACK", "RELEASE", "SPREAD", "BPM", "PLAY MODE"};
        const std::array<std::string, 5> values{percent(synth.parameter(SynthParameter::Attack)), percent(synth.parameter(SynthParameter::Release)),
            percent(synth.parameter(SynthParameter::Spread)), std::to_string(p.bpm), mode_name(p.mode)};
        const std::array<float, 5> bars{synth.parameter(SynthParameter::Attack), synth.parameter(SynthParameter::Release), synth.parameter(SynthParameter::Spread), -1.0F, -1.0F};
        for (int i = 0; i < 5; ++i) parameter_row(r, x + 10, y + 54 + i * 43, w - 20, names[static_cast<std::size_t>(i)], values[static_cast<std::size_t>(i)], i == row, kOrange, bars[static_cast<std::size_t>(i)]);
    }
}

const std::array<std::string, 8> kFxBankA{"REVERSE-ish", "STUTTER", "CHOP", "CRUSH", "DRIVE", "WASH", "DEEP ECHO", "PHASE"};
const std::array<std::string, 8> kFxBankB{"ABYSS", "RATETRAP", "GLITCH", "BROKEN", "DOOM", "SHIMMER-ish", "TUNNEL", "ALIEN"};

int direction_index(Direction d) {
    switch (d) {
    case Direction::UpLeft: return 0; case Direction::Up: return 1; case Direction::UpRight: return 2; case Direction::Right: return 3;
    case Direction::DownRight: return 4; case Direction::Down: return 5; case Direction::DownLeft: return 6; case Direction::Left: return 7;
    case Direction::Center: break;
    }
    return -1;
}

void draw_fx_mode(SDL_Renderer* r, const UiState& ui, const InputState& in) {
    constexpr int x = 12, y = 46, w = 238, h = 286;
    const int layer = in.l1 ? 1 : 0;
    const auto& names = layer == 0 ? kFxBankA : kFxBankB;
    panel_frame(r, x, y, w, h, kTeal);
    text(r, x + 12, y + 10, "PERF FX", 2, kTeal);
    micro(r, x + w - 12, y + 14, layer == 0 ? "L1: ALT" : "L1 HELD", layer == 1 ? kWhite : kDim, true);
    micro(r, x + 12, y + 35, "HOLD DPAD / RELEASE RESTORES", kDim);
    const int gx = x + 10, gy = y + 58, cw = 68, ch = 56, gap = 6;
    const auto positions = grid_positions(gx, gy, cw, ch, gap);
    for (const auto& item : positions) {
        const int idx = direction_index(item.first);
        const bool live = ui.perf_fx_active && ui.perf_fx_direction == item.first && ui.perf_fx_layer == layer;
        draw_cell(r, item.second.first, item.second.second, cw, ch, names[static_cast<std::size_t>(idx)], false, live, kTeal);
    }
    draw_cell(r, gx + cw + gap, gy + ch + gap, cw, ch, "DRY", false, !ui.perf_fx_active, kTeal);
    micro(r, x + w / 2, y + h - 18, layer == 0 ? "BANK A: CUT / DIRT / SPACE" : "BANK B: EXTREME COMBOS", kInk, true);
}

void chord_button(SDL_Renderer* r, int x, int y, int w, int h, const std::string& key,
                  const std::string& label, bool active, Colour accent, float pulse) {
    Colour fill = active ? accent : kPanel2;
    if (active && pulse > 0.5F) fill = kWhite;
    rect(r, x, y, w, h, fill); outline(r, x, y, w, h, active ? kWhite : alpha(accent, 140));
    text(r, x + 8, y + 7, key, 2, active && pulse <= 0.5F ? kBg : accent);
    text(r, x + w / 2, y + h - 17, fit(label, 12), 1, active && pulse <= 0.5F ? kBg : kInk, true, 0);
}

void draw_chord_panel(SDL_Renderer* r, const PerformanceState& p, const UiState& ui,
                      const InputState& in, const std::optional<ChordSpec>& current, const SynthEngine& synth) {
    constexpr int x = 262, y = 46, w = 238, h = 286;
    const int bank = active_bank(in, ui);
    const Colour accent = bank == 2 ? kBlue : bank == 1 ? kGreen : kPurpleBright;
    panel_frame(r, x, y, w, h, accent);
    text(r, x + 12, y + 10, bank_name(bank), 2, accent);
    std::string modifier = ui.selected_direction == Direction::Center ? "BASE" : direction_label(ui.selected_palette, ui.selected_direction);
    micro(r, x + w - 12, y + 14, fit(modifier, 17), kInk, true);
    micro(r, x + 12, y + 35, "R1 / R2 HOLD CHANGES BANK", kDim);

    const float pulse = 0.5F + 0.5F * std::sin(static_cast<float>(SDL_GetTicks()) * 0.012F);
    chord_button(r, x + 83, y + 58, 72, 58, "X", face_label(p.key_pc, Face::Y, bank), in.face[3], kGreen, pulse);
    chord_button(r, x + 12, y + 125, 72, 58, "Y", face_label(p.key_pc, Face::X, bank), in.face[2], kBlue, pulse);
    chord_button(r, x + 154, y + 125, 72, 58, "A", face_label(p.key_pc, Face::B, bank), in.face[1], kRed, pulse);
    chord_button(r, x + 83, y + 192, 72, 58, "B", face_label(p.key_pc, Face::A, bank), in.face[0], kOrange, pulse);

    const std::string chord = current ? current->degree + "  " + current->name : "READY";
    text(r, x + w / 2, y + h - 28, fit(chord, 22), current ? 2 : 1, current ? kWhite : kDim, true, 0);
    bar(r, x + 12, y + h - 9, w - 24, synth.output_peak(), accent);
}

constexpr int kSettingsCount = 16;
std::string settings_name(int row) {
    static const std::array<std::string, kSettingsCount> names{
        "BASE COLOUR", "L1 COLOUR", "BASE BANK", "R1 BANK", "R2 BANK",
        "FX1 TYPE", "FX1 AMOUNT", "FX1 COLOUR", "FX2 TYPE", "FX2 AMOUNT", "FX2 COLOUR",
        "KEY", "OCTAVE", "UI MOTION", "SWAP L1/L2", "SWAP R1/R2"};
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
    case 13: return ui_motion_name(ui.ui_motion); case 14: return on_off(ui.swap_left_rear); case 15: return on_off(ui.swap_right_rear);
    default: return "";
    }
}

void draw_settings(SDL_Renderer* r, const PerformanceState& p, const UiState& ui, const SynthEngine& synth) {
    rect(r, 0, 0, kWidth, kHeight, alpha(kBg, 248));
    panel_frame(r, 12, 12, 488, 360, kBlue);
    text(r, 28, 28, "SETTINGS", 3, kBlue);
    micro(r, 486, 38, "SELECT: CLOSE", kDim, true);
    micro(r, 28, 58, "UP/DOWN SELECT  LEFT/RIGHT CHANGE", kDim);
    const int visible = 7;
    const int start = std::clamp(ui.settings_scroll, 0, kSettingsCount - visible);
    for (int i = 0; i < visible; ++i) {
        const int row = start + i; const bool active = row == ui.settings_row;
        rect(r, 28, 78 + i * 38, 456, 31, active ? kBlue : kPanel2);
        outline(r, 28, 78 + i * 38, 456, 31, active ? kWhite : alpha(kBlue, 100));
        text(r, 40, 88 + i * 38, settings_name(row), 1, active ? kBg : kInk, false, 0);
        text(r, 452, 88 + i * 38, fit(settings_value(row, p, ui, synth), 22), 1, active ? kBg : kDim, true, 0);
    }
    micro(r, 28, 352, "START+SELECT: SAVE & EXIT", kDim);
    micro(r, 484, 352, std::to_string(ui.settings_row + 1) + "/" + std::to_string(kSettingsCount), kDim, true);
}

void draw_footer(SDL_Renderer* r, const UiState& ui, const InputState& in) {
    rect(r, 0, 340, kWidth, 44, kPanel);
    if (ui.dpad_mode == DpadMode::Chord) {
        micro(r, 14, 350, "L1 HOLD ALT COLOUR", kInk);
        micro(r, 14, 366, "L2: SOUND / PERF FX", kDim);
    } else if (ui.dpad_mode == DpadMode::Sound) {
        micro(r, 14, 350, "DPAD EDITS SOUND", kInk);
        micro(r, 14, 366, "L1 HOLD SECOND PARAM BANK", kDim);
    } else {
        micro(r, 14, 350, "HOLD DPAD FOR PERFORMANCE FX", kInk);
        micro(r, 14, 366, "L1 HOLD SECOND FX BANK", kDim);
    }
    micro(r, 286, 350, "R1/R2 HOLD CHORD BANKS", kInk);
    micro(r, 286, 366, "SELECT SETTINGS  START MODE", kDim);
    if (in.l1) rect(r, 238, 347, 8, 8, kWhite);
}

void draw_ui(SDL_Renderer* r, const PerformanceState& p, const UiState& ui, const InputState& in,
             const std::optional<ChordSpec>& current, const SynthEngine& synth) {
    draw_background(r, ui, synth.output_peak());
    draw_header(r, p, ui, synth.output_peak());
    if (ui.dpad_mode == DpadMode::Chord) draw_chord_mode(r, ui, in);
    else if (ui.dpad_mode == DpadMode::Sound) draw_sound_mode(r, p, ui, in, synth);
    else draw_fx_mode(r, ui, in);
    draw_chord_panel(r, p, ui, in, current, synth);
    draw_footer(r, ui, in);
    if (!ui.toast.empty() && SDL_GetTicks() < ui.toast_until) {
        const int tw = std::min(420, text_width(ui.toast, 2, 1) + 28);
        rect(r, (kWidth - tw) / 2, 164, tw, 44, kBgLift);
        outline(r, (kWidth - tw) / 2, 164, tw, 44, mode_accent(ui.dpad_mode));
        text(r, kWidth / 2, 179, fit(ui.toast, 32), 2, kWhite, true);
    }
    if (ui.settings) draw_settings(r, p, ui, synth);
}

std::optional<Face> face_button(SDL_GameControllerButton button) {
    switch (button) {
    case SDL_CONTROLLER_BUTTON_A: return Face::A; // physical B, bottom
    case SDL_CONTROLLER_BUTTON_B: return Face::B; // physical A, right
    case SDL_CONTROLLER_BUTTON_X: return Face::X; // physical Y, left
    case SDL_CONTROLLER_BUTTON_Y: return Face::Y; // physical X, top
    default: return std::nullopt;
    }
}
void set_dpad(InputState& in, SDL_GameControllerButton button, bool down) {
    if (button == SDL_CONTROLLER_BUTTON_DPAD_UP) in.up = down;
    if (button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) in.down = down;
    if (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) in.left = down;
    if (button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) in.right = down;
}

void update_chord(PerformanceState& p, UiState& ui, InputState& in,
                  std::optional<ChordSpec>& current, std::vector<int>& previous,
                  SynthEngine& synth) {
    if (!in.active_face) return;
    const int bank = active_bank(in, ui);
    p.chord_bank = bank;
    p.colour_palette = ui.selected_palette;
    const auto chord = make_chord(p.key_pc, p.octave, *in.active_face, bank,
                                  ui.selected_palette, ui.selected_direction);
    const auto notes = voice_lead(chord, previous);
    previous = notes; current = chord; synth.play_chord(notes);
}
void release_face(InputState& in, Face face, std::optional<ChordSpec>& current, SynthEngine& synth) {
    in.face[static_cast<std::size_t>(face)] = false;
    if (in.active_face && *in.active_face == face) {
        in.active_face.reset();
        for (int i = 3; i >= 0; --i) if (in.face[static_cast<std::size_t>(i)]) { in.active_face = static_cast<Face>(i); break; }
        if (!in.active_face) { current.reset(); synth.release_chord(); }
    }
}

EffectSettings fx(EffectType type, float amount, float colour_value) {
    EffectSettings out; out.type = type; out.amount = amount; out.colour = colour_value; return out;
}
std::pair<EffectSettings, EffectSettings> performance_fx(int layer, Direction d) {
    if (layer == 0) {
        switch (d) {
        case Direction::UpLeft: return {fx(EffectType::Delay, 0.82F, 0.12F), fx(EffectType::Crusher, 0.42F, 0.20F)};
        case Direction::Up: return {fx(EffectType::Tremolo, 1.00F, 0.92F), fx(EffectType::Off, 0.0F, 0.5F)};
        case Direction::UpRight: return {fx(EffectType::Tremolo, 1.00F, 1.00F), fx(EffectType::Crusher, 0.45F, 0.18F)};
        case Direction::Right: return {fx(EffectType::Crusher, 0.95F, 0.15F), fx(EffectType::Drive, 0.72F, 0.35F)};
        case Direction::DownRight: return {fx(EffectType::Drive, 0.98F, 0.24F), fx(EffectType::Phaser, 0.58F, 0.78F)};
        case Direction::Down: return {fx(EffectType::Reverb, 1.00F, 0.92F), fx(EffectType::Chorus, 0.72F, 0.82F)};
        case Direction::DownLeft: return {fx(EffectType::Delay, 1.00F, 0.94F), fx(EffectType::Reverb, 0.74F, 0.88F)};
        case Direction::Left: return {fx(EffectType::Phaser, 1.00F, 0.88F), fx(EffectType::Chorus, 0.82F, 0.76F)};
        case Direction::Center: break;
        }
    } else {
        switch (d) {
        case Direction::UpLeft: return {fx(EffectType::Reverb, 1.00F, 1.00F), fx(EffectType::Delay, 1.00F, 0.98F)};
        case Direction::Up: return {fx(EffectType::Tremolo, 1.00F, 0.99F), fx(EffectType::Delay, 0.56F, 0.10F)};
        case Direction::UpRight: return {fx(EffectType::Crusher, 1.00F, 0.06F), fx(EffectType::Tremolo, 0.86F, 0.95F)};
        case Direction::Right: return {fx(EffectType::Crusher, 0.88F, 0.04F), fx(EffectType::Phaser, 1.00F, 0.96F)};
        case Direction::DownRight: return {fx(EffectType::Drive, 1.00F, 0.08F), fx(EffectType::Reverb, 0.82F, 0.32F)};
        case Direction::Down: return {fx(EffectType::Chorus, 1.00F, 1.00F), fx(EffectType::Reverb, 1.00F, 0.96F)};
        case Direction::DownLeft: return {fx(EffectType::Delay, 1.00F, 1.00F), fx(EffectType::Phaser, 0.90F, 0.14F)};
        case Direction::Left: return {fx(EffectType::Phaser, 1.00F, 0.05F), fx(EffectType::Crusher, 0.62F, 0.08F)};
        case Direction::Center: break;
        }
    }
    return {fx(EffectType::Off, 0.0F, 0.5F), fx(EffectType::Off, 0.0F, 0.5F)};
}
void restore_performance_fx(UiState& ui, SynthEngine& synth) {
    if (!ui.perf_fx_active) return;
    synth.set_effect(0, ui.fx_restore[0]); synth.set_effect(1, ui.fx_restore[1]);
    ui.perf_fx_active = false; ui.perf_fx_direction = Direction::Center;
}
void update_performance_fx(UiState& ui, const InputState& in, SynthEngine& synth) {
    if (ui.settings || ui.dpad_mode != DpadMode::Fx) { restore_performance_fx(ui, synth); return; }
    const Direction d = held_direction(in);
    if (d == Direction::Center) { restore_performance_fx(ui, synth); return; }
    const int layer = in.l1 ? 1 : 0;
    if (!ui.perf_fx_active) {
        ui.fx_restore[0] = synth.effect(0); ui.fx_restore[1] = synth.effect(1);
    }
    if (ui.perf_fx_active && ui.perf_fx_direction == d && ui.perf_fx_layer == layer) return;
    const auto pair = performance_fx(layer, d);
    synth.set_effect(0, pair.first); synth.set_effect(1, pair.second);
    ui.perf_fx_active = true; ui.perf_fx_direction = d; ui.perf_fx_layer = layer;
}

void change_octave(PerformanceState& p, UiState& ui, int delta, InputState& in,
                   std::optional<ChordSpec>& current, std::vector<int>& previous, SynthEngine& synth) {
    p.octave = std::clamp(p.octave + delta, -2, 2);
    toast(ui, "OCTAVE " + std::string(p.octave >= 0 ? "+" : "") + std::to_string(p.octave));
    if (in.active_face) update_chord(p, ui, in, current, previous, synth);
}

void cycle_dpad_mode(UiState& ui, InputState& in, SynthEngine& synth) {
    restore_performance_fx(ui, synth);
    const int count = static_cast<int>(DpadMode::Count);
    ui.dpad_mode = static_cast<DpadMode>((static_cast<int>(ui.dpad_mode) + 1) % count);
    ui.held_edit = 0;
    toast(ui, "DPAD: " + dpad_mode_name(ui.dpad_mode));
    if (ui.dpad_mode == DpadMode::Fx) update_performance_fx(ui, in, synth);
}

void edit_sound(UiState& ui, PerformanceState& p, const InputState& in, SynthEngine& synth, int delta) {
    const int layer = in.l1 ? 1 : 0;
    const int row = ui.sound_row[static_cast<std::size_t>(layer)];
    if (layer == 0) {
        if (row == 0) { p.preset = (p.preset + delta + SynthEngine::preset_count()) % SynthEngine::preset_count(); synth.set_preset(p.preset); toast(ui, synth.preset_info().name); }
        else { const auto param = static_cast<SynthParameter>(row - 1); synth.set_parameter(param, synth.parameter(param) + static_cast<float>(delta) * 0.02F); }
    } else {
        if (row <= 2) { const auto param = static_cast<SynthParameter>(row + static_cast<int>(SynthParameter::Attack)); synth.set_parameter(param, synth.parameter(param) + static_cast<float>(delta) * 0.02F); }
        else if (row == 3) { p.bpm = std::clamp(p.bpm + delta, 40, 240); synth.set_bpm(p.bpm); }
        else { const int mode = (static_cast<int>(p.mode) + delta + 4) % 4; p.mode = static_cast<PlayMode>(mode); synth.set_mode(p.mode); }
    }
}
void move_sound_row(UiState& ui, const InputState& in, int delta) {
    const int layer = in.l1 ? 1 : 0; const int count = layer == 0 ? 4 : 5;
    int& row = ui.sound_row[static_cast<std::size_t>(layer)]; row = (row + delta + count) % count;
}

void edit_setting(UiState& ui, PerformanceState& p, SynthEngine& synth, int delta) {
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
    else if (row == 13) ui.ui_motion = std::clamp(ui.ui_motion + delta, 0, 2);
    else if (row == 14) ui.swap_left_rear = !ui.swap_left_rear;
    else if (row == 15) ui.swap_right_rear = !ui.swap_right_rear;
}
void move_settings_row(UiState& ui, int delta) {
    ui.settings_row = (ui.settings_row + delta + kSettingsCount) % kSettingsCount;
    const int visible = 7;
    if (ui.settings_row < ui.settings_scroll) ui.settings_scroll = ui.settings_row;
    if (ui.settings_row >= ui.settings_scroll + visible) ui.settings_scroll = ui.settings_row - visible + 1;
}

void handle_dpad_press(UiState& ui, PerformanceState& p, InputState& in, SynthEngine& synth,
                       SDL_GameControllerButton button, std::optional<ChordSpec>& current,
                       std::vector<int>& previous) {
    const bool up = button == SDL_CONTROLLER_BUTTON_DPAD_UP;
    const bool down = button == SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    const bool left = button == SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    const bool right = button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    if (ui.settings) {
        if (up) move_settings_row(ui, -1);
        if (down) move_settings_row(ui, 1);
        if (left || right) edit_setting(ui, p, synth, right ? 1 : -1);
    } else if (ui.dpad_mode == DpadMode::Chord) {
        const Direction d = held_direction(in);
        if (d != Direction::Center) {
            const int palette = display_palette(in, ui);
            if (ui.selected_palette == palette && ui.selected_direction == d) ui.selected_direction = Direction::Center;
            else { ui.selected_palette = palette; ui.selected_direction = d; }
            toast(ui, ui.selected_direction == Direction::Center ? "BASE" : palette_name(ui.selected_palette) + " / " + direction_label(ui.selected_palette, ui.selected_direction));
            if (in.active_face) update_chord(p, ui, in, current, previous, synth);
        }
    } else if (ui.dpad_mode == DpadMode::Sound) {
        if (up) move_sound_row(ui, in, -1);
        if (down) move_sound_row(ui, in, 1);
        if (left || right) edit_sound(ui, p, in, synth, right ? 1 : -1);
    }
    if ((ui.settings || ui.dpad_mode == DpadMode::Sound) && (left || right)) {
        ui.held_edit = right ? 1 : -1; ui.held_since = SDL_GetTicks(); ui.last_repeat = ui.held_since;
    }
}

void repeat_edit(UiState& ui, PerformanceState& p, const InputState& in, SynthEngine& synth) {
    if (ui.held_edit == 0) return;
    const Uint32 now = SDL_GetTicks(); const Uint32 age = now - ui.held_since;
    const Uint32 interval = age > 2200U ? 40U : age > 850U ? 90U : 100000U;
    if (now - ui.last_repeat < interval) return;
    if (ui.settings) edit_setting(ui, p, synth, ui.held_edit);
    else if (ui.dpad_mode == DpadMode::Sound) edit_sound(ui, p, in, synth, ui.held_edit);
    ui.last_repeat = now;
}

void set_l1(UiState& ui, InputState& in, bool down, SynthEngine& synth) {
    if (in.l1 == down) return;
    in.l1 = down;
    if (ui.dpad_mode == DpadMode::Fx) update_performance_fx(ui, in, synth);
}
void set_l2(UiState& ui, InputState& in, bool down, SynthEngine& synth) {
    if (in.l2 == down) return;
    in.l2 = down;
    if (down && !ui.settings) cycle_dpad_mode(ui, in, synth);
}
void set_r1(UiState& ui, PerformanceState& p, InputState& in, bool down,
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
    const bool logical_l1 = first != ui.swap_left_rear;
    if (logical_l1) set_l1(ui, in, down, synth); else set_l2(ui, in, down, synth);
    if (in.active_face) update_chord(p, ui, in, current, previous, synth);
}
void set_right_physical(UiState& ui, PerformanceState& p, InputState& in, bool first, bool down,
                        std::optional<ChordSpec>& current, std::vector<int>& previous, SynthEngine& synth) {
    const bool logical_r1 = first != ui.swap_right_rear;
    if (logical_r1) set_r1(ui, p, in, down, current, previous, synth); else set_r2(ui, p, in, down, current, previous, synth);
}

void log_controller(SDL_GameController* controller) {
    if (!controller) return;
    SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);
    std::cerr << "controller: " << SDL_GameControllerName(controller)
              << " buttons=" << SDL_JoystickNumButtons(joystick)
              << " axes=" << SDL_JoystickNumAxes(joystick)
              << " hats=" << SDL_JoystickNumHats(joystick) << '\n';
    if (char* mapping = SDL_GameControllerMapping(controller)) { std::cerr << "mapping: " << mapping << '\n'; SDL_free(mapping); }
    std::cerr << "roles: shoulders=front octave, leftstick=L1, lefttrigger=L2, rightstick=R1, righttrigger=R2\n";
}

} // namespace

int main(int, char**) {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS) != 0) {
        std::cerr << "SDL init: " << SDL_GetError() << '\n'; return 1;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_Window* window = SDL_CreateWindow("BRKCHRD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) { std::cerr << "window: " << SDL_GetError() << '\n'; SDL_Quit(); return 1; }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(renderer, kWidth, kHeight);

    SynthEngine synth(kRate); PerformanceState state; UiState ui;
    load_config(state, ui, synth); synth.set_mode(state.mode); synth.set_bpm(state.bpm); synth.set_latch(false);

    SDL_AudioSpec desired{}; desired.freq = kRate; desired.format = AUDIO_F32SYS; desired.channels = 2;
    desired.samples = kFrames; desired.callback = audio_callback; desired.userdata = &synth;
    SDL_AudioSpec obtained{}; const SDL_AudioDeviceID audio = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (audio != 0) SDL_PauseAudioDevice(audio, 0); else std::cerr << "audio: " << SDL_GetError() << '\n';

    SDL_GameController* controller = nullptr;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) if (SDL_IsGameController(i)) { controller = SDL_GameControllerOpen(i); if (controller) break; }
    log_controller(controller);

    InputState input; std::optional<ChordSpec> current; std::vector<int> previous;
    bool running = true; int raw_log_budget = 96;
    toast(ui, "BRKCHRD 0.4  PERFORMANCE WORKFLOW", 1500U);

    while (running) {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_CONTROLLERDEVICEADDED && !controller) { controller = SDL_GameControllerOpen(event.cdevice.which); log_controller(controller); }
            if (event.type == SDL_CONTROLLERDEVICEREMOVED && controller &&
                SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller)) == event.cdevice.which) {
                SDL_GameControllerClose(controller); controller = nullptr;
            }

            if ((event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYAXISMOTION) && raw_log_budget > 0) {
                if (event.type == SDL_JOYBUTTONDOWN) { std::cerr << "raw button down: " << static_cast<int>(event.jbutton.button) << '\n'; --raw_log_budget; }
                else if (std::abs(event.jaxis.value) > 15000) { std::cerr << "raw axis: " << static_cast<int>(event.jaxis.axis) << " value=" << event.jaxis.value << '\n'; --raw_log_budget; }
            }

            if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP) {
                const bool down = event.type == SDL_CONTROLLERBUTTONDOWN;
                const auto button = static_cast<SDL_GameControllerButton>(event.cbutton.button);
                if (down && raw_log_budget > 0) {
                    const char* name = SDL_GameControllerGetStringForButton(button);
                    std::cerr << "controller button down: " << static_cast<int>(button) << ' ' << (name ? name : "?") << '\n'; --raw_log_budget;
                }
                if (auto face = face_button(button)) {
                    if (down) { input.face[static_cast<std::size_t>(*face)] = true; input.active_face = *face; update_chord(state, ui, input, current, previous, synth); }
                    else { release_face(input, *face, current, synth); if (input.active_face) update_chord(state, ui, input, current, previous, synth); }
                } else if (button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) {
                    input.front_left = down; if (down) change_octave(state, ui, -1, input, current, previous, synth);
                } else if (button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) {
                    input.front_right = down; if (down) change_octave(state, ui, 1, input, current, previous, synth);
                } else if (button == SDL_CONTROLLER_BUTTON_LEFTSTICK) {
                    set_left_physical(ui, state, input, true, down, current, previous, synth);
                } else if (button == SDL_CONTROLLER_BUTTON_RIGHTSTICK) {
                    set_right_physical(ui, state, input, true, down, current, previous, synth);
                } else if (button == SDL_CONTROLLER_BUTTON_DPAD_UP || button == SDL_CONTROLLER_BUTTON_DPAD_DOWN ||
                           button == SDL_CONTROLLER_BUTTON_DPAD_LEFT || button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
                    set_dpad(input, button, down);
                    if (ui.dpad_mode == DpadMode::Fx && !ui.settings) update_performance_fx(ui, input, synth);
                    else if (down) handle_dpad_press(ui, state, input, synth, button, current, previous);
                    if (!down && (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT || button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) ui.held_edit = 0;
                } else if (button == SDL_CONTROLLER_BUTTON_START) {
                    if (down) { input.start = true; input.start_long = false; input.start_since = SDL_GetTicks(); if (input.select) running = false; }
                    else {
                        if (!input.start_long && !input.select) { state.mode = static_cast<PlayMode>((static_cast<int>(state.mode) + 1) % 4); synth.set_mode(state.mode); toast(ui, mode_name(state.mode)); }
                        input.start = false;
                    }
                } else if (button == SDL_CONTROLLER_BUTTON_BACK) {
                    if (down) { input.select = true; if (input.start) running = false; }
                    else {
                        if (!input.start) { restore_performance_fx(ui, synth); ui.settings = !ui.settings; ui.held_edit = 0; toast(ui, ui.settings ? "SETTINGS" : "BACK TO PLAY"); if (!ui.settings) save_config(state,  ui, synth); }
                        input.select = false;
                    }
                }
            }

            if (event.type == SDL_CONTROLLERAXISMOTION) {
                const bool down = event.caxis.value > 15000;
                if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) set_left_physical(ui, state, input, false, down, current, previous, synth);
                if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) set_right_physical(ui, state, input, false, down, current, previous, synth);
            }

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                const bool down = event.type == SDL_KEYDOWN; const SDL_Keycode key = event.key.keysym.sym;
                auto keyboard_face = [&](Face face) {
                    if (down) { input.face[static_cast<std::size_t>(face)] = true; input.active_face = face; update_chord(state, ui, input, current, previous, synth); }
                    else { release_face(input, face, current, synth); if (input.active_face) update_chord(state, ui, input, current, previous, synth); }
                };
                if (key == SDLK_z) keyboard_face(Face::A);
                else if (key == SDLK_x) keyboard_face(Face::B);
                else if (key == SDLK_a) keyboard_face(Face::X);
                else if (key == SDLK_s) keyboard_face(Face::Y);
                else if (key == SDLK_q) set_l1(ui, input, down, synth);
                else if (key == SDLK_w && down && event.key.repeat == 0) set_l2(ui, input, true, synth);
                else if (key == SDLK_w && !down) set_l2(ui, input, false, synth);
                else if (key == SDLK_e) set_r1(ui, state, input, down, current, previous, synth);
                else if (key == SDLK_r) set_r2(ui, state, input, down, current, previous, synth);
                else if (key == SDLK_LEFTBRACKET && down && event.key.repeat == 0) change_octave(state, ui, -1, input, current, previous, synth);
                else if (key == SDLK_RIGHTBRACKET && down && event.key.repeat == 0) change_octave(state, ui, 1, input, current, previous, synth);
                else if (key == SDLK_UP || key == SDLK_DOWN || key == SDLK_LEFT || key == SDLK_RIGHT) {
                    const auto button = key == SDLK_UP ? SDL_CONTROLLER_BUTTON_DPAD_UP : key == SDLK_DOWN ? SDL_CONTROLLER_BUTTON_DPAD_DOWN : key == SDLK_LEFT ? SDL_CONTROLLER_BUTTON_DPAD_LEFT : SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
                    set_dpad(input, button, down);
                    if (ui.dpad_mode == DpadMode::Fx && !ui.settings) update_performance_fx(ui, input, synth);
                    else if (down) handle_dpad_press(ui, state, input, synth, button, current, previous);
                    if (!down && (key == SDLK_LEFT || key == SDLK_RIGHT)) ui.held_edit = 0;
                } else if (key == SDLK_RETURN) {
                    if (down && event.key.repeat == 0) { input.start = true; input.start_since = SDL_GetTicks(); input.start_long = false; }
                    else if (!down) { if (!input.start_long) { state.mode = static_cast<PlayMode>((static_cast<int>(state.mode) + 1) % 4); synth.set_mode(state.mode); } input.start = false; }
                } else if (key == SDLK_SPACE && down && event.key.repeat == 0) {
                    restore_performance_fx(ui, synth); ui.settings = !ui.settings; if (!ui.settings) save_config(state, ui, synth);
                } else if (key == SDLK_ESCAPE && down) running = false;
            }
        }

        if (input.start && !input.select && !input.start_long && SDL_GetTicks() - input.start_since > 850U) {
            input.start_long = true; synth.all_notes_off(); current.reset(); input.active_face.reset(); input.face.fill(false); toast(ui, "ALL NOTES OFF", 1200U);
        }
        repeat_edit(ui, state, input, synth);
        if (SDL_GetTicks() >= ui.toast_until) ui.toast.clear();
        draw_ui(renderer, state, ui, input, current, synth);
        SDL_RenderPresent(renderer); SDL_Delay(8);
    }

    restore_performance_fx(ui, synth); save_config(state, ui, synth); synth.all_notes_off();
    if (audio != 0) SDL_CloseAudioDevice(audio);
    if (controller) SDL_GameControllerClose(controller);
    SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit(); return 0;
}
