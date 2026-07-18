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
#include <vector>

using namespace brkchrd;

namespace {

constexpr int kWidth = 512;
constexpr int kHeight = 384;
constexpr int kRate = 48000;
constexpr int kFrames = 512;

struct Colour { Uint8 r, g, b, a = 255; };
constexpr Colour kBg{8, 7, 12};
constexpr Colour kInk{238, 226, 197};
constexpr Colour kDim{151, 143, 158};
constexpr Colour kPurple{117, 67, 171};
constexpr Colour kPanel{35, 30, 46};
constexpr Colour kPanel2{48, 41, 62};
constexpr Colour kRed{216, 88, 88};
constexpr Colour kOrange{224, 154, 63};
constexpr Colour kTeal{80, 169, 154};
constexpr Colour kBlue{91, 122, 187};
constexpr Colour kGreen{112, 188, 120};

void colour(SDL_Renderer* renderer, Colour value) { SDL_SetRenderDrawColor(renderer, value.r, value.g, value.b, value.a); }
void rect(SDL_Renderer* renderer, int x, int y, int w, int h, Colour value) {
    colour(renderer, value); SDL_Rect area{x,y,w,h}; SDL_RenderFillRect(renderer, &area);
}
void outline(SDL_Renderer* renderer, int x, int y, int w, int h, Colour value) {
    colour(renderer, value); SDL_Rect area{x,y,w,h}; SDL_RenderDrawRect(renderer, &area);
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

int text_width(std::string_view value, int scale = 1) { return value.empty() ? 0 : static_cast<int>(value.size()) * 6 * scale - scale; }
std::string fit(std::string value, int chars) {
    if (chars < 1) return {};
    if (static_cast<int>(value.size()) <= chars) return value;
    if (chars <= 3) return value.substr(0, static_cast<std::size_t>(chars));
    return value.substr(0, static_cast<std::size_t>(chars - 3)) + "...";
}
void text(SDL_Renderer* renderer, int x, int y, std::string_view value, int scale, Colour value_colour, bool centered = false) {
    if (centered) x -= text_width(value, scale) / 2;
    colour(renderer, value_colour);
    int cursor = x;
    for (char ch : value) {
        const auto rows = glyph(ch);
        for (int row = 0; row < 7; ++row) for (int column = 0; column < 5; ++column) {
            if (((rows[static_cast<std::size_t>(row)] >> (4 - column)) & 1U) != 0U) {
                SDL_Rect pixel{cursor + column * scale, y + row * scale, scale, scale}; SDL_RenderFillRect(renderer, &pixel);
            }
        }
        cursor += 6 * scale;
    }
}
void wrapped(SDL_Renderer* renderer, int x, int y, int chars, const std::string& source, Colour value, int max_lines = 4) {
    std::istringstream in(source); std::string word; std::string line; int row = 0;
    while (in >> word && row < max_lines) {
        if (!line.empty() && static_cast<int>(line.size() + 1U + word.size()) > chars) {
            text(renderer, x, y + row * 12, fit(line, chars), 1, value); ++row; line.clear();
        }
        if (!line.empty()) line += ' ';
        line += word;
    }
    if (!line.empty() && row < max_lines) text(renderer, x, y + row * 12, fit(line, chars), 1, value);
}

void bar(SDL_Renderer* renderer, int x, int y, int w, float value, Colour active) {
    rect(renderer, x, y, w, 6, kBg); outline(renderer, x, y, w, 6, kDim);
    rect(renderer, x + 1, y + 1, static_cast<int>(static_cast<float>(w - 2) * std::clamp(value, 0.0F, 1.0F)), 4, active);
}

enum class Page { Play = 0, Sound, Fx, System, Count };

struct InputState {
    std::array<bool,4> face{};
    bool up = false, down = false, left = false, right = false;
    bool rear_l = false, rear_r = false;
    bool fx1 = false, fx2 = false;
    bool start = false, select = false;
    bool rear_l_used = false, rear_r_used = false;
    std::optional<Face> active_face;
};

struct UiState {
    Page page = Page::Play;
    std::array<int,4> selected{};
    Direction armed = Direction::Center;
    int palette = 0;
    Uint32 toast_until = 0U;
    std::string toast;
    int held_edit = 0;
    Uint32 held_since = 0U;
    Uint32 last_repeat = 0U;
};

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
int bank(const InputState& input) { return input.rear_r ? 2 : input.rear_l ? 1 : 0; }
Direction performance_direction(const InputState& input, const UiState& ui) {
    const Direction live = held_direction(input); return live == Direction::Center ? ui.armed : live;
}

std::filesystem::path config_path() {
    if (const char* root = std::getenv("XDG_CONFIG_HOME")) return std::filesystem::path(root) / "brkchrd.cfg";
    if (const char* home = std::getenv("HOME")) return std::filesystem::path(home) / ".config" / "brkchrd.cfg";
    return "brkchrd.cfg";
}

void load_config(PerformanceState& performance, UiState& ui, SynthEngine& synth) {
    std::ifstream in(config_path()); std::string key; float value = 0.0F;
    std::array<float, static_cast<std::size_t>(SynthParameter::Count)> params{};
    std::array<bool, static_cast<std::size_t>(SynthParameter::Count)> param_seen{};
    std::array<EffectSettings,2> effects{}; std::array<bool,2> effect_seen{};
    while (in >> key >> value) {
        if (key == "key") performance.key_pc = std::clamp(static_cast<int>(value), 0, 11);
        else if (key == "octave") performance.octave = std::clamp(static_cast<int>(value), -2, 2);
        else if (key == "preset") performance.preset = static_cast<int>(value);
        else if (key == "bpm") performance.bpm = std::clamp(static_cast<int>(value), 40, 240);
        else if (key == "mode") performance.mode = static_cast<PlayMode>(std::clamp(static_cast<int>(value), 0, 3));
        else if (key == "latch") performance.latch = value != 0.0F;
        else if (key == "palette") ui.palette = std::clamp(static_cast<int>(value), 0, 2);
        else if (key == "direction") ui.armed = static_cast<Direction>(std::clamp(static_cast<int>(value), 0, 8));
        else if (key.rfind("param",0) == 0) {
            const int index = std::clamp(std::stoi(key.substr(5)), 0, static_cast<int>(SynthParameter::Count) - 1);
            params[static_cast<std::size_t>(index)] = value; param_seen[static_cast<std::size_t>(index)] = true;
        } else if (key == "fx1type") { effects[0].type = static_cast<EffectType>(std::clamp(static_cast<int>(value),0,SynthEngine::effect_count()-1)); effect_seen[0] = true; }
        else if (key == "fx1amount") { effects[0].amount = value; effect_seen[0] = true; }
        else if (key == "fx1colour") { effects[0].colour = value; effect_seen[0] = true; }
        else if (key == "fx2type") { effects[1].type = static_cast<EffectType>(std::clamp(static_cast<int>(value),0,SynthEngine::effect_count()-1)); effect_seen[1] = true; }
        else if (key == "fx2amount") { effects[1].amount = value; effect_seen[1] = true; }
        else if (key == "fx2colour") { effects[1].colour = value; effect_seen[1] = true; }
    }
    synth.set_preset(performance.preset); performance.preset = synth.preset_index();
    for (int i = 0; i < static_cast<int>(SynthParameter::Count); ++i) if (param_seen[static_cast<std::size_t>(i)]) synth.set_parameter(static_cast<SynthParameter>(i), params[static_cast<std::size_t>(i)]);
    for (int slot = 0; slot < 2; ++slot) if (effect_seen[static_cast<std::size_t>(slot)]) synth.set_effect(slot, effects[static_cast<std::size_t>(slot)]);
}

void save_config(const PerformanceState& performance, const UiState& ui, const SynthEngine& synth) {
    const auto path = config_path(); std::error_code error; std::filesystem::create_directories(path.parent_path(), error);
    std::ofstream out(path);
    out << "key " << performance.key_pc << '\n' << "octave " << performance.octave << '\n' << "preset " << synth.preset_index() << '\n';
    out << "bpm " << performance.bpm << '\n' << "mode " << static_cast<int>(performance.mode) << '\n' << "latch " << (performance.latch ? 1 : 0) << '\n';
    out << "palette " << ui.palette << '\n' << "direction " << static_cast<int>(ui.armed) << '\n';
    for (int i = 0; i < static_cast<int>(SynthParameter::Count); ++i) out << "param" << i << ' ' << synth.parameter(static_cast<SynthParameter>(i)) << '\n';
    for (int slot = 0; slot < 2; ++slot) {
        const auto fx = synth.effect(slot); const int number = slot + 1;
        out << "fx" << number << "type " << static_cast<int>(fx.type) << '\n';
        out << "fx" << number << "amount " << fx.amount << '\n';
        out << "fx" << number << "colour " << fx.colour << '\n';
    }
}

void audio_callback(void* userdata, Uint8* bytes, int count) {
    auto* synth = static_cast<SynthEngine*>(userdata); auto* output = reinterpret_cast<float*>(bytes);
    synth->render(output, static_cast<std::size_t>(count) / (sizeof(float) * 2U));
}

std::string page_name(Page page) {
    switch (page) { case Page::Play: return "PLAY"; case Page::Sound: return "SOUND"; case Page::Fx: return "FX"; case Page::System: return "SYSTEM"; case Page::Count: break; }
    return "PLAY";
}
Colour page_colour(Page page) { return page == Page::Play ? kPurple : page == Page::Sound ? kOrange : page == Page::Fx ? kTeal : kBlue; }

void header(SDL_Renderer* renderer, Page page, float peak) {
    rect(renderer,0,0,kWidth,34,kPanel);
    text(renderer,12,8,"BRKCHRD",2,kInk);
    constexpr std::array<Page,4> pages{Page::Play,Page::Sound,Page::Fx,Page::System};
    int x = 142;
    for (Page item : pages) {
        const int w = item == Page::System ? 78 : 66; const bool active = item == page;
        if (active) rect(renderer,x,4,w,26,page_colour(item));
        text(renderer,x + w/2,11,page_name(item),1,active ? kBg : kDim,true); x += w + 4;
    }
    rect(renderer,472,8,28,12,kBg); rect(renderer,474,10,static_cast<int>(24.0F * std::clamp(peak,0.0F,1.0F)),8,peak > 0.82F ? kRed : kGreen);
}

void chord_button(SDL_Renderer* renderer, int x, int y, int w, int h, const std::string& key, const std::string& label, bool active, Colour accent) {
    rect(renderer,x,y,w,h,active ? accent : kPanel2); if (!active) outline(renderer,x,y,w,h,accent);
    text(renderer,x+8,y+7,key,2,active ? kBg : accent);
    text(renderer,x+w/2,y+h-17,fit(label,13),1,active ? kBg : kInk,true);
}

void draw_play(SDL_Renderer* renderer, const PerformanceState& state, const UiState& ui, const InputState& input, const std::optional<ChordSpec>& current, const SynthEngine& synth) {
    rect(renderer,12,44,210,270,kPanel); rect(renderer,234,44,266,270,kPanel);
    text(renderer,24,56,"CHORD COLOUR",1,kDim); text(renderer,210,56,palette_name(ui.palette),1,page_colour(Page::Play),true);
    const int cx=117, cy=174;
    const std::array<std::pair<Direction,std::pair<int,int>>,8> positions{{
        {Direction::Up,{cx,cy-78}}, {Direction::UpRight,{cx+65,cy-56}}, {Direction::Right,{cx+76,cy}}, {Direction::DownRight,{cx+64,cy+56}},
        {Direction::Down,{cx,cy+78}}, {Direction::DownLeft,{cx-64,cy+56}}, {Direction::Left,{cx-76,cy}}, {Direction::UpLeft,{cx-65,cy-56}},
    }};
    const Direction shown = performance_direction(input,ui);
    rect(renderer,cx-30,cy-14,60,28,shown == Direction::Center ? kPurple : kPanel2);
    text(renderer,cx,cy-4,"BASE",1,shown == Direction::Center ? kBg : kInk,true);
    for (const auto& [direction,position] : positions) {
        const bool active = shown == direction; const int x=position.first-29, y=position.second-11;
        rect(renderer,x,y,58,22,active ? kPurple : kPanel2); if (!active) outline(renderer,x,y,58,22,kDim);
        text(renderer,position.first,position.second-4,fit(direction_label(ui.palette,direction),9),1,active?kBg:kInk,true);
    }
    text(renderer,117,286,"TAP SAME DIR = BASE",1,kDim,true);
    text(renderer,117,299,"FX1 EXT  FX2 DARK",1,kDim,true);

    text(renderer,248,56,"CHORD BANK",1,kDim); text(renderer,486,56,bank_name(bank(input)),1,page_colour(Page::Play),true);
    chord_button(renderer,333,83,102,52,"Y",face_label(state.key_pc,Face::Y,bank(input)),input.face[3],kGreen);
    chord_button(renderer,250,145,102,52,"X",face_label(state.key_pc,Face::X,bank(input)),input.face[2],kBlue);
    chord_button(renderer,416,145,72,52,"B",face_label(state.key_pc,Face::B,bank(input)),input.face[1],kRed);
    chord_button(renderer,333,207,102,52,"A",face_label(state.key_pc,Face::A,bank(input)),input.face[0],kOrange);
    const auto preset=synth.preset_info();
    text(renderer,367,274,fit(preset.name,24),2,kInk,true); text(renderer,367,298,fit(preset.engine,30),1,kDim,true);

    rect(renderer,12,324,488,48,kPanel);
    if (current) {
        text(renderer,24,333,fit(current->degree + " " + current->name,18),3,kOrange);
        text(renderer,286,333,fit(current->modifier,16),2,kInk);
    } else {
        text(renderer,24,333,"ARM DPAD THEN PLAY",2,kInk);
        text(renderer,286,333,"A B X Y",2,kOrange);
    }
    text(renderer,24,358,"L/R TAP PAGE  HOLD+CHORD BANK",1,kDim);
    text(renderer,286,358,"START MODE  SELECT LATCH",1,state.latch?kOrange:kDim);
}

std::string percent(float value) { return std::to_string(static_cast<int>(std::round(value * 100.0F))) + "%"; }
void parameter_row(SDL_Renderer* renderer, int x, int y, int w, const std::string& name, const std::string& value, bool active, Colour accent, float bar_value = -1.0F) {
    rect(renderer,x,y,w,25,active ? accent : kPanel2);
    text(renderer,x+8,y+7,fit(name,18),1,active?kBg:kInk);
    text(renderer,x+w-8-text_width(fit(value,14),1),y+7,fit(value,14),1,active?kBg:kDim);
    if (bar_value >= 0.0F) bar(renderer,x+w-90,y+17,80,bar_value,active?kBg:accent);
}

void draw_sound(SDL_Renderer* renderer, const PerformanceState& state, const UiState& ui, const SynthEngine& synth) {
    rect(renderer,12,44,300,270,kPanel); rect(renderer,324,44,176,270,kPanel);
    const auto info=synth.preset_info(); const int selected=ui.selected[static_cast<int>(Page::Sound)];
    const std::array<std::string,9> names{"PRESET","TONE","BODY","MOTION","ATTACK","RELEASE","SPREAD","BPM","PLAY MODE"};
    std::array<std::string,9> values{
        info.name, percent(synth.parameter(SynthParameter::Tone)), percent(synth.parameter(SynthParameter::Body)),
        percent(synth.parameter(SynthParameter::Motion)), percent(synth.parameter(SynthParameter::Attack)),
        percent(synth.parameter(SynthParameter::Release)), percent(synth.parameter(SynthParameter::Spread)),
        std::to_string(state.bpm), mode_name(state.mode)};
    for (int row=0; row<9; ++row) {
        float value=-1.0F; if (row>=1 && row<=6) value=synth.parameter(static_cast<SynthParameter>(row-1));
        parameter_row(renderer,24,56+row*27,276,names[static_cast<std::size_t>(row)],values[static_cast<std::size_t>(row)],row==selected,kOrange,value);
    }
    text(renderer,412,58,fit(info.name,20),2,kOrange,true); text(renderer,412,86,fit(info.engine,22),1,kInk,true);
    wrapped(renderer,340,112,23,info.character,kDim,3);
    text(renderer,340,166,"SOUND MACROS",1,kDim);
    for (int i=0;i<6;++i) bar(renderer,340,184+i*16,142,synth.parameter(static_cast<SynthParameter>(i)), i%2==0?kOrange:kPurple);
    text(renderer,340,286,"DPAD UP/DN SELECT",1,kDim); text(renderer,340,300,"DPAD LT/RT EDIT",1,kDim);
    rect(renderer,12,324,488,48,kPanel); text(renderer,24,334,"A/B/X/Y ALWAYS PLAY",2,kInk); text(renderer,286,336,"L/R TAP PAGE",1,kDim); text(renderer,286,354,"START MODE  SELECT LATCH",1,kDim);
}

void draw_fx(SDL_Renderer* renderer, const UiState& ui, const SynthEngine& synth) {
    rect(renderer,12,44,300,270,kPanel); rect(renderer,324,44,176,270,kPanel);
    const int selected=ui.selected[static_cast<int>(Page::Fx)]; const auto one=synth.effect(0); const auto two=synth.effect(1);
    const std::array<std::string,7> names{"FX1 TYPE","FX1 AMOUNT","FX1 COLOUR","FX2 TYPE","FX2 AMOUNT","FX2 COLOUR","MASTER"};
    const std::array<std::string,7> values{SynthEngine::effect_name(one.type),percent(one.amount),percent(one.colour),SynthEngine::effect_name(two.type),percent(two.amount),percent(two.colour),percent(synth.parameter(SynthParameter::Master))};
    for (int row=0;row<7;++row) {
        float value=-1.0F; if (row==1)value=one.amount; if(row==2)value=one.colour; if(row==4)value=two.amount; if(row==5)value=two.colour; if(row==6)value=synth.parameter(SynthParameter::Master);
        parameter_row(renderer,24,64+row*34,276,names[static_cast<std::size_t>(row)],values[static_cast<std::size_t>(row)],row==selected,kTeal,value);
    }
    text(renderer,412,58,"SERIAL CHAIN",2,kTeal,true);
    rect(renderer,342,94,140,50,kPanel2); text(renderer,412,104,"FX1",2,kRed,true); text(renderer,412,127,fit(SynthEngine::effect_name(one.type),18),1,kInk,true);
    text(renderer,412,154,"V",2,kDim,true);
    rect(renderer,342,180,140,50,kPanel2); text(renderer,412,190,"FX2",2,kOrange,true); text(renderer,412,213,fit(SynthEngine::effect_name(two.type),18),1,kInk,true);
    text(renderer,412,240,"V",2,kDim,true); rect(renderer,342,266,140,30,kPanel2); text(renderer,412,276,"MASTER",1,kInk,true);
    rect(renderer,12,324,488,48,kPanel); text(renderer,24,334,"8 EFFECT TYPES / 2 SLOTS",2,kInk); text(renderer,286,336,"DPAD EDIT",1,kDim); text(renderer,286,354,"A/B/X/Y PLAY THROUGH FX",1,kDim);
}

void draw_system(SDL_Renderer* renderer, const PerformanceState& state, const UiState& ui) {
    rect(renderer,12,44,300,270,kPanel); rect(renderer,324,44,176,270,kPanel);
    const int selected=ui.selected[static_cast<int>(Page::System)];
    const std::array<std::string,5> names{"KEY","OCTAVE","RESET COLOUR","PANIC","ABOUT"};
    const std::array<std::string,5> values{note_name(state.key_pc),std::to_string(state.octave),"ACTION","ACTION","V0.2.0"};
    for(int row=0;row<5;++row) parameter_row(renderer,24,64+row*42,276,names[static_cast<std::size_t>(row)],values[static_cast<std::size_t>(row)],row==selected,kBlue);
    text(renderer,412,58,"HARDWARE",2,kBlue,true); wrapped(renderer,340,90,22,"TRIMUI BRICK KNULI SCARAB  AARCH64",kInk,4);
    text(renderer,340,150,"INPUT COMPAT",1,kDim); wrapped(renderer,340,168,22,"FX1 FX2 ACCEPT L3 R3 AND TRIGGER AXES",kInk,5);
    text(renderer,340,238,"SAVE AND EXIT",1,kDim); text(renderer,340,256,"START + SELECT",1,kOrange);
    rect(renderer,12,324,488,48,kPanel); text(renderer,24,334,"KEY/OCTAVE LIVE HERE",2,kInk); text(renderer,286,336,"PLAY DPAD NEVER TRANSPOSES",1,kDim); text(renderer,286,354,"NO MORE ACCIDENTAL KEY JUMPS",1,kDim);
}

void draw_ui(SDL_Renderer* renderer, const PerformanceState& state, const UiState& ui, const InputState& input, const std::optional<ChordSpec>& current, const SynthEngine& synth) {
    rect(renderer,0,0,kWidth,kHeight,kBg); header(renderer,ui.page,synth.output_peak());
    switch(ui.page) { case Page::Play: draw_play(renderer,state,ui,input,current,synth); break; case Page::Sound: draw_sound(renderer,state,ui,synth); break; case Page::Fx: draw_fx(renderer,ui,synth); break; case Page::System: draw_system(renderer,state,ui); break; case Page::Count: break; }
    if (!ui.toast.empty() && SDL_GetTicks() < ui.toast_until) {
        const int w=std::min(360,text_width(ui.toast,2)+28); rect(renderer,(kWidth-w)/2,168,w,42,kBg); outline(renderer,(kWidth-w)/2,168,w,42,page_colour(ui.page));
        text(renderer,kWidth/2,181,fit(ui.toast,27),2,page_colour(ui.page),true);
    }
}

std::optional<Face> face_button(SDL_GameControllerButton button) {
    switch(button) { case SDL_CONTROLLER_BUTTON_A:return Face::A; case SDL_CONTROLLER_BUTTON_B:return Face::B; case SDL_CONTROLLER_BUTTON_X:return Face::X; case SDL_CONTROLLER_BUTTON_Y:return Face::Y; default:return std::nullopt; }
}

void set_dpad(InputState& input, SDL_GameControllerButton button, bool down) {
    if (button == SDL_CONTROLLER_BUTTON_DPAD_UP) input.up = down;
    if (button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) input.down = down;
    if (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) input.left = down;
    if (button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) input.right = down;
}

void update_chord(PerformanceState& state, UiState& ui, InputState& input, std::optional<ChordSpec>& current, std::vector<int>& previous, SynthEngine& synth) {
    if (!input.active_face) return;
    state.chord_bank = bank(input);
    state.colour_palette = ui.palette;
    const auto chord=make_chord(state.key_pc,state.octave,*input.active_face,state.chord_bank,state.colour_palette,performance_direction(input,ui));
    const auto notes=voice_lead(chord,previous); previous=notes; current=chord; synth.play_chord(notes);
}
void release_face(InputState& input, Face face, std::optional<ChordSpec>& current, SynthEngine& synth) {
    input.face[static_cast<std::size_t>(face)]=false;
    if(input.active_face && *input.active_face==face) {
        input.active_face.reset(); for(int i=3;i>=0;--i) if(input.face[static_cast<std::size_t>(i)]) {input.active_face=static_cast<Face>(i);break;}
        if(!input.active_face){current.reset();synth.release_chord();}
    }
}

void change_page(UiState& ui, int delta) {
    int page=static_cast<int>(ui.page)+delta; const int count=static_cast<int>(Page::Count); page=(page%count+count)%count; ui.page=static_cast<Page>(page);
}
void toast(UiState& ui, std::string value) { ui.toast=std::move(value); ui.toast_until=SDL_GetTicks()+1000U; }

void edit(UiState& ui, PerformanceState& state, SynthEngine& synth, int delta) {
    if(ui.page==Page::Sound) {
        const int row=ui.selected[static_cast<int>(Page::Sound)];
        if(row==0){state.preset=(state.preset+delta+SynthEngine::preset_count())%SynthEngine::preset_count();synth.set_preset(state.preset);toast(ui,synth.preset_info().name);}
        else if(row>=1&&row<=6){const auto parameter=static_cast<SynthParameter>(row-1);synth.set_parameter(parameter,synth.parameter(parameter)+static_cast<float>(delta) * 0.02F);}
        else if(row==7){state.bpm=std::clamp(state.bpm+delta,40,240);synth.set_bpm(state.bpm);}
        else if(row==8){int mode=(static_cast<int>(state.mode)+delta+4)%4;state.mode=static_cast<PlayMode>(mode);synth.set_mode(state.mode);}
    } else if(ui.page==Page::Fx) {
        const int row=ui.selected[static_cast<int>(Page::Fx)];
        if(row<=2){auto fx=synth.effect(0);if(row==0){int type=(static_cast<int>(fx.type)+delta+SynthEngine::effect_count())%SynthEngine::effect_count();fx.type=static_cast<EffectType>(type);}if(row==1)fx.amount+=static_cast<float>(delta) * 0.02F;if(row==2)fx.colour+=static_cast<float>(delta) * 0.02F;synth.set_effect(0,fx);}
        else if(row<=5){auto fx=synth.effect(1);if(row==3){int type=(static_cast<int>(fx.type)+delta+SynthEngine::effect_count())%SynthEngine::effect_count();fx.type=static_cast<EffectType>(type);}if(row==4)fx.amount+=static_cast<float>(delta) * 0.02F;if(row==5)fx.colour+=static_cast<float>(delta) * 0.02F;synth.set_effect(1,fx);}
        else synth.set_parameter(SynthParameter::Master,synth.parameter(SynthParameter::Master)+static_cast<float>(delta) * 0.02F);
    } else if(ui.page==Page::System) {
        const int row=ui.selected[static_cast<int>(Page::System)];
        if(row==0)state.key_pc=(state.key_pc+delta+12)%12; else if(row==1)state.octave=std::clamp(state.octave+delta,-2,2);
        else if(row==2){ui.armed=Direction::Center;ui.palette=0;toast(ui,"COLOUR RESET");}
        else if(row==3){synth.all_notes_off();toast(ui,"PANIC");}
    }
}
void move_selection(UiState& ui, int delta) {
    int count=1; if(ui.page==Page::Sound)count=9; else if(ui.page==Page::Fx)count=7; else if(ui.page==Page::System)count=5;
    int& selected=ui.selected[static_cast<int>(ui.page)]; selected=(selected+delta+count)%count;
}
void handle_dpad_press(UiState& ui, PerformanceState& state, InputState& input, SynthEngine& synth, SDL_GameControllerButton button) {
    if(ui.page==Page::Play) {
        const Direction direction=held_direction(input); if(direction!=Direction::Center) ui.armed=ui.armed==direction?Direction::Center:direction;
    } else {
        if (button == SDL_CONTROLLER_BUTTON_DPAD_UP) move_selection(ui, -1);
        if (button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) move_selection(ui, 1);
        if(button==SDL_CONTROLLER_BUTTON_DPAD_LEFT){edit(ui,state,synth,-1);ui.held_edit=-1;ui.held_since=SDL_GetTicks();ui.last_repeat=ui.held_since;}
        if(button==SDL_CONTROLLER_BUTTON_DPAD_RIGHT){edit(ui,state,synth,1);ui.held_edit=1;ui.held_since=SDL_GetTicks();ui.last_repeat=ui.held_since;}
    }
}
void set_fx(UiState& ui, InputState& input, bool first, bool down, std::optional<ChordSpec>& current, PerformanceState& state, std::vector<int>& previous, SynthEngine& synth) {
    if(first)input.fx1=down;else input.fx2=down;
    if(down) {
        if(input.fx1&&input.fx2){ui.palette=0;ui.armed=Direction::Center;toast(ui,"BASE / CLASSIC");}
        else {const int wanted=first?1:2;ui.palette=ui.palette==wanted?0:wanted;toast(ui,palette_name(ui.palette));}
    }
    if(input.active_face)update_chord(state,ui,input,current,previous,synth);
}

} // namespace

int main(int, char**) {
    SDL_SetMainReady();
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_GAMECONTROLLER|SDL_INIT_JOYSTICK|SDL_INIT_EVENTS)!=0){std::cerr<<"SDL init: "<<SDL_GetError()<<'\n';return 1;}
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");
    SDL_Window* window=SDL_CreateWindow("BRKCHRD",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1024,768,SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    if(!window){std::cerr<<"window: "<<SDL_GetError()<<'\n';SDL_Quit();return 1;}
    SDL_Renderer* renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);if(!renderer)renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
    SDL_RenderSetLogicalSize(renderer,kWidth,kHeight);

    SynthEngine synth(kRate); PerformanceState state; UiState ui; load_config(state,ui,synth); synth.set_mode(state.mode);synth.set_bpm(state.bpm);synth.set_latch(state.latch);
    SDL_AudioSpec desired{};desired.freq=kRate;desired.format=AUDIO_F32SYS;desired.channels=2;desired.samples=kFrames;desired.callback=audio_callback;desired.userdata=&synth;
    SDL_AudioSpec obtained{};const SDL_AudioDeviceID audio=SDL_OpenAudioDevice(nullptr,0,&desired,&obtained,0);if(audio!=0)SDL_PauseAudioDevice(audio,0);else std::cerr<<"audio: "<<SDL_GetError()<<'\n';

    SDL_GameController* controller=nullptr;for(int i=0;i<SDL_NumJoysticks();++i)if(SDL_IsGameController(i)){controller=SDL_GameControllerOpen(i);if(controller)break;}
    if(controller)std::cerr<<"controller: "<<SDL_GameControllerName(controller)<<'\n';

    InputState input;std::optional<ChordSpec> current;std::vector<int> previous;bool running=true;toast(ui,"PRODUCT PREVIEW 0.2.0");
    while(running) {
        SDL_Event event{};
        while(SDL_PollEvent(&event)) {
            if(event.type==SDL_QUIT)running=false;
            if(event.type==SDL_CONTROLLERDEVICEADDED&&!controller)controller=SDL_GameControllerOpen(event.cdevice.which);
            if(event.type==SDL_CONTROLLERDEVICEREMOVED&&controller&&SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))==event.cdevice.which){SDL_GameControllerClose(controller);controller=nullptr;}
            if(event.type==SDL_CONTROLLERBUTTONDOWN||event.type==SDL_CONTROLLERBUTTONUP) {
                const bool down=event.type==SDL_CONTROLLERBUTTONDOWN;const auto button=static_cast<SDL_GameControllerButton>(event.cbutton.button);
                if(auto face=face_button(button)) {
                    if(down){input.face[static_cast<std::size_t>(*face)]=true;input.active_face=*face;if(input.rear_l)input.rear_l_used=true;if(input.rear_r)input.rear_r_used=true;update_chord(state,ui,input,current,previous,synth);}
                    else{release_face(input,*face,current,synth);if(input.active_face)update_chord(state,ui,input,current,previous,synth);}
                } else if(button==SDL_CONTROLLER_BUTTON_LEFTSTICK||button==SDL_CONTROLLER_BUTTON_RIGHTSTICK) {
                    set_fx(ui,input,button==SDL_CONTROLLER_BUTTON_LEFTSTICK,down,current,state,previous,synth);
                } else {
                    if(button==SDL_CONTROLLER_BUTTON_DPAD_UP||button==SDL_CONTROLLER_BUTTON_DPAD_DOWN||button==SDL_CONTROLLER_BUTTON_DPAD_LEFT||button==SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
                        set_dpad(input,button,down);if(down){if(input.active_face)update_chord(state,ui,input,current,previous,synth);else handle_dpad_press(ui,state,input,synth,button);}else{if(input.active_face)update_chord(state,ui,input,current,previous,synth);if(button==SDL_CONTROLLER_BUTTON_DPAD_LEFT||button==SDL_CONTROLLER_BUTTON_DPAD_RIGHT)ui.held_edit=0;}
                    } else if(button==SDL_CONTROLLER_BUTTON_LEFTSHOULDER) {
                        input.rear_l=down;if(down)input.rear_l_used=false;else if(!input.rear_l_used&&!input.active_face)change_page(ui,-1);if(input.active_face){input.rear_l_used=true;update_chord(state,ui,input,current,previous,synth);}
                    } else if(button==SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) {
                        input.rear_r=down;if(down)input.rear_r_used=false;else if(!input.rear_r_used&&!input.active_face)change_page(ui,1);if(input.active_face){input.rear_r_used=true;update_chord(state,ui,input,current,previous,synth);}
                    } else if(button==SDL_CONTROLLER_BUTTON_START) {
                        input.start=down;if(!down&&!input.select){state.mode=static_cast<PlayMode>((static_cast<int>(state.mode)+1)%4);synth.set_mode(state.mode);toast(ui,mode_name(state.mode));}
                    } else if(button==SDL_CONTROLLER_BUTTON_BACK) {
                        input.select=down;if(!down&&!input.start){state.latch=!state.latch;synth.set_latch(state.latch);toast(ui,state.latch?"LATCH ON":"LATCH OFF");}
                    }
                    if(input.start&&input.select)running=false;
                }
            }
            if(event.type==SDL_CONTROLLERAXISMOTION) {
                const bool down=event.caxis.value>15000;
                if(event.caxis.axis==SDL_CONTROLLER_AXIS_TRIGGERLEFT&&input.fx1!=down)set_fx(ui,input,true,down,current,state,previous,synth);
                if(event.caxis.axis==SDL_CONTROLLER_AXIS_TRIGGERRIGHT&&input.fx2!=down)set_fx(ui,input,false,down,current,state,previous,synth);
            }
            if(event.type==SDL_KEYDOWN||event.type==SDL_KEYUP) {
                const bool down=event.type==SDL_KEYDOWN;const SDL_Keycode key=event.key.keysym.sym;
                auto keyboard_face=[&](Face face){if(down){input.face[static_cast<std::size_t>(face)]=true;input.active_face=face;update_chord(state,ui,input,current,previous,synth);}else release_face(input,face,current,synth);};
                if(key==SDLK_z)keyboard_face(Face::A);else if(key==SDLK_x)keyboard_face(Face::B);else if(key==SDLK_a)keyboard_face(Face::X);else if(key==SDLK_s)keyboard_face(Face::Y);
                else if(key==SDLK_q){input.rear_l=down;if(!down&&!input.active_face)change_page(ui,-1);if(input.active_face)update_chord(state,ui,input,current,previous,synth);}
                else if(key==SDLK_w){input.rear_r=down;if(!down&&!input.active_face)change_page(ui,1);if(input.active_face)update_chord(state,ui,input,current,previous,synth);}
                else if(key==SDLK_e)set_fx(ui,input,true,down,current,state,previous,synth);else if(key==SDLK_r)set_fx(ui,input,false,down,current,state,previous,synth);
                else if(key==SDLK_UP||key==SDLK_DOWN||key==SDLK_LEFT||key==SDLK_RIGHT){SDL_GameControllerButton button=key==SDLK_UP?SDL_CONTROLLER_BUTTON_DPAD_UP:key==SDLK_DOWN?SDL_CONTROLLER_BUTTON_DPAD_DOWN:key==SDLK_LEFT?SDL_CONTROLLER_BUTTON_DPAD_LEFT:SDL_CONTROLLER_BUTTON_DPAD_RIGHT;set_dpad(input,button,down);if(down){if(input.active_face)update_chord(state,ui,input,current,previous,synth);else handle_dpad_press(ui,state,input,synth,button);}else{if(input.active_face)update_chord(state,ui,input,current,previous,synth);ui.held_edit=0;}}
                else if(key==SDLK_RETURN&&down&&event.key.repeat==0){state.mode=static_cast<PlayMode>((static_cast<int>(state.mode)+1)%4);synth.set_mode(state.mode);toast(ui,mode_name(state.mode));}
                else if(key==SDLK_SPACE&&down&&event.key.repeat==0){state.latch=!state.latch;synth.set_latch(state.latch);toast(ui,state.latch?"LATCH ON":"LATCH OFF");}
                else if(key==SDLK_ESCAPE&&down)running=false;
            }
        }
        if(ui.held_edit!=0&&ui.page!=Page::Play){const Uint32 now=SDL_GetTicks();const Uint32 age=now-ui.held_since;const Uint32 interval=age>2200U?40U:age>850U?90U:100000U;if(now-ui.last_repeat>=interval){edit(ui,state,synth,ui.held_edit);ui.last_repeat=now;}}
        if (SDL_GetTicks() >= ui.toast_until) ui.toast.clear();
        draw_ui(renderer, state, ui, input, current, synth);
        SDL_RenderPresent(renderer);
        SDL_Delay(8);
    }
    save_config(state,ui,synth);synth.all_notes_off();if(audio!=0)SDL_CloseAudioDevice(audio);if(controller)SDL_GameControllerClose(controller);SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);SDL_Quit();return 0;
}
