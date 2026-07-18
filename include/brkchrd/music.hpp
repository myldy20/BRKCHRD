#pragma once

#include <array>
#include <string>
#include <vector>

namespace brkchrd {

enum class Face { A = 0, B = 1, X = 2, Y = 3 };
enum class Direction { Center, Up, UpRight, Right, DownRight, Down, DownLeft, Left, UpLeft };
enum class PlayMode { Pad = 0, Strum = 1, Arp = 2, Pulse = 3 };

struct ChordSpec {
    int root_pc = 0;
    int base_octave = 3;
    std::vector<int> intervals;
    std::string name;
    std::string degree;
    std::string modifier;
};

struct PerformanceState {
    int key_pc = 0;
    int octave = 0;
    int chord_bank = 0;
    int colour_palette = 0;
    int preset = 0;
    int bpm = 92;
    PlayMode mode = PlayMode::Pad;
    bool latch = false;
};

ChordSpec make_chord(int key_pc,
                     int octave,
                     Face face,
                     int chord_bank,
                     int colour_palette,
                     Direction direction);

std::vector<int> voice_lead(const ChordSpec& chord, const std::vector<int>& previous);
std::string face_label(int key_pc, Face face, int chord_bank);
std::string direction_label(int colour_palette, Direction direction);
std::string note_name(int pitch_class);
std::string mode_name(PlayMode mode);
std::string bank_name(int bank);
std::string palette_name(int palette);

} // namespace brkchrd
