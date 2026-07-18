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
#include <vector>

using namespace brkchrd;

namespace {

constexpr int kLogicalW = 512;
constexpr int kLogicalH = 384;
constexpr int kAudioRate = 48000;
constexpr int kAudioFrames = 512;

struct Colour {
    Uint8 r, g, b, a = 255;
};

constexpr Colour kBg{18, 20, 24};
constexpr Colour kPanel{31, 34, 40};
constexpr Colour kPanel2{42, 46, 54};
constexpr Colour kText{232, 235, 239};
constexpr Colour kDim{133, 143, 155};
constexpr Colour kAccent{238, 198, 92};
constexpr Colour kBlue{99, 175, 218};
constexpr Colour kGreen{105, 204, 154};
constexpr Colour kRed{224, 105, 105};

void set_colour(SDL_Renderer* renderer, Colour c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
}

void fill_rect(SDL_Renderer* renderer, int x, int y, int w, int h, Colour c) {
    set_colour(renderer, c);
    SDL_Rect rect{x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, Colour c) {
    set_colour(renderer, c);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

std::array<std::uint8_t, 7> glyph(char ch) {
    ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    switch (ch) {
        case 'A': return {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11};
        case 'B': return {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E};
        case 'C': return {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E};
        case 'D': return {0x1E,0x11,0x11,0x11,0x11,0x11,0x1E};
        case 'E': return {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F};
        case 'F': return {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10};
        case 'G': return {0x0E,0x11,0x10,0x17,0x11,0x11,0x0E};
        case 'H': return {0x11,0x11,0x11,0x1F,0x11,0x11,0x11};
        case 'I': return {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E};
        case 'J': return {0x07,0x02,0x02,0x02,0x12,0x12,0x0C};
        case 'K': return {0x11,0x12,0x14,0x18,0x14,0x12,0x11};
        case 'L': return {0x10,0x10,0x10,0x10,0x10,0x10,0x1F};
        case 'M': return {0x11,0x1B,0x15,0x15,0x11,0x11,0x11};
        case 'N': return {0x11,0x19,0x15,0x13,0x11,0x11,0x11};
        case 'O': return {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E};
        case 'P': return {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10};
        case 'Q': return {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D};
        case 'R': return {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11};
        case 'S': return {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E};
        case 'T': return {0x1F,0x04,0x04,0x04,0x04,0x04,0x04};
        case 'U': return {0x11,0x11,0x11,0x11,0x11,0x11,0x0E};
        case 'V': return {0x11,0x11,0x11,0x11,0x11,0x0A,0x04};
        case 'W': return {0x11,0x11,0x11,0x15,0x15,0x15,0x0A};
        case 'X': return {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11};
        case 'Y': return {0x11,0x11,0x0A,0x04,0x04,0x04,0x04};
        case 'Z': return {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F};
        case '0': return {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E};
        case '1': return {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E};
        case '2': return {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F};
        case '3': return {0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E};
        case '4': return {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02};
        case '5': return {0x1F,0x10,0x10,0x1E,0x01,0x01,0x1E};
        case '6': return {0x0E,0x10,0x10,0x1E,0x11,0x11,0x0E};
        case '7': return {0x1F,0x01,0x02,0x04,0x08,0x08,0x08};
        case '8': return {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E};
        case '9': return {0x0E,0x11,0x11,0x0F,0x01,0x01,0x0E};
        case '#': return {0x0A,0x1F,0x0A,0x0A,0x1F,0x0A,0x00};
        case '+': return {0x00,0x04,0x04,0x1F,0x04,0x04,0x00};
        case '-': return {0x00,0x00,0x00,0x1F,0x00,0x00,0x00};
        case '/': return {0x01,0x02,0x02,0x04,0x08,0x08,0x10};
        case '.': return {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C};
        case ':': return {0x00,0x0C,0x0C,0x00,0x0C,0x0C,0x00};
        case '<': return {0x02,0x04,0x08,0x10,0x08,0x04,0x02};
        case '>': return {0x08,0x04,0x02,0x01,0x02,0x04,0x08};
        case '[': return {0x0E,0x08,0x08,0x08,0x08,0x08,0x0E};
        case ']': return {0x0E,0x02,0x02,0x02,0x02,0x02,0x0E};
        case ' ': return {0,0,0,0,0,0,0};
        default: return {0x1F,0x11,0x02,0x04,0x00,0x04,0x00};
    }
}

void text(SDL_Renderer* renderer, int x, int y, const std::string& value, int scale, Colour c, bool centered = false) {
    const int width = static_cast<int>(value.size()) * 6 * scale - scale;
    if (centered) x -= width / 2;
    set_colour(renderer, c);
    int cursor = x;
    for (char ch : value) {
        const auto rows = glyph(ch);
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if ((rows[static_cast<std::size_t>(row)] >> (4 - col)) & 1u) {
                    SDL_Rect pixel{cursor + col * scale, y + row * scale, scale, scale};
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        cursor += 6 * scale;
    }
}

std::string compact(std::string s) {
    for (char& c : s) {
        if (static_cast<unsigned char>(c) >= 128) c = 'b';
    }
    return s;
}

struct InputState {
    std::array<bool, 4> face{};
    bool d_up = false;
    bool d_down = false;
    bool d_left = false;
    bool d_right = false;
    bool l1 = false;
    bool r1 = false;
    bool l2 = false;
    bool r2 = false;
    bool start = false;
    bool select = false;
    std::optional<Face> active_face;
};

Direction direction_from(const InputState& input) {
    if (input.d_up && input.d_left) return Direction::UpLeft;
    if (input.d_up && input.d_right) return Direction::UpRight;
    if (input.d_down && input.d_left) return Direction::DownLeft;
    if (input.d_down && input.d_right) return Direction::DownRight;
    if (input.d_up) return Direction::Up;
    if (input.d_down) return Direction::Down;
    if (input.d_left) return Direction::Left;
    if (input.d_right) return Direction::Right;
    return Direction::Center;
}

int bank_from(const InputState& input) {
    if (input.r1) return 2;
    if (input.l1) return 1;
    return 0;
}

int palette_from(const InputState& input) {
    if (input.r2) return 2;
    if (input.l2) return 1;
    return 0;
}

std::filesystem::path config_path() {
    if (const char* xdg = std::getenv("XDG_CONFIG_HOME")) return std::filesystem::path(xdg) / "brkchrd.cfg";
    if (const char* home = std::getenv("HOME")) return std::filesystem::path(home) / ".config" / "brkchrd.cfg";
    return "brkchrd.cfg";
}

void load_config(PerformanceState& state) {
    std::ifstream in(config_path());
    std::string key;
    int value = 0;
    while (in >> key >> value) {
        if (key == "key") state.key_pc = std::clamp(value, 0, 11);
        else if (key == "octave") state.octave = std::clamp(value, -2, 2);
        else if (key == "preset") state.preset = value;
        else if (key == "bpm") state.bpm = std::clamp(value, 40, 240);
        else if (key == "mode") state.mode = static_cast<PlayMode>(std::clamp(value, 0, 3));
        else if (key == "latch") state.latch = value != 0;
    }
}

void save_config(const PerformanceState& state) {
    const auto path = config_path();
    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);
    std::ofstream out(path);
    out << "key " << state.key_pc << '\n';
    out << "octave " << state.octave << '\n';
    out << "preset " << state.preset << '\n';
    out << "bpm " << state.bpm << '\n';
    out << "mode " << static_cast<int>(state.mode) << '\n';
    out << "latch " << (state.latch ? 1 : 0) << '\n';
}

void audio_callback(void* userdata, Uint8* stream, int len) {
    auto* synth = static_cast<SynthEngine*>(userdata);
    auto* output = reinterpret_cast<float*>(stream);
    const std::size_t frames = static_cast<std::size_t>(len) / (sizeof(float) * 2);
    synth->render(output, frames);
}

void draw_button(SDL_Renderer* renderer, int x, int y, int w, int h, const std::string& key, const std::string& label, bool active, Colour colour) {
    fill_rect(renderer, x, y, w, h, active ? colour : kPanel2);
    text(renderer, x + 8, y + 7, key, 2, active ? kBg : colour);
    text(renderer, x + w / 2, y + 28, compact(label), 2, active ? kBg : kText, true);
}
