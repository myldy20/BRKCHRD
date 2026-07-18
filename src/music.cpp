#include "brkchrd/music.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numeric>

namespace brkchrd {
namespace {

enum class Quality { Major, Minor, Diminished };

struct BaseChord {
    int root_offset = 0;
    Quality quality = Quality::Major;
    const char* degree = "I";
};

constexpr std::array<int, 7> kMajorScale{0, 2, 4, 5, 7, 9, 11};

BaseChord base_for(Face face, int bank) {
    const int f = static_cast<int>(face);
    if (bank == 1) {
        constexpr std::array<BaseChord, 4> kBank{{
            {2, Quality::Minor, "ii"},
            {4, Quality::Minor, "iii"},
            {11, Quality::Diminished, "viiO"},
            {10, Quality::Major, "bVII"},
        }};
        return kBank[static_cast<std::size_t>(f)];
    }
    if (bank == 2) {
        constexpr std::array<BaseChord, 4> kBank{{
            {0, Quality::Minor, "i"},
            {5, Quality::Minor, "iv"},
            {8, Quality::Major, "bVI"},
            {3, Quality::Major, "bIII"},
        }};
        return kBank[static_cast<std::size_t>(f)];
    }
    constexpr std::array<BaseChord, 4> kBank{{
        {0, Quality::Major, "I"},
        {7, Quality::Major, "V"},
        {9, Quality::Minor, "vi"},
        {5, Quality::Major, "IV"},
    }};
    return kBank[static_cast<std::size_t>(f)];
}

std::vector<int> triad(Quality q) {
    switch (q) {
        case Quality::Major: return {0, 4, 7};
        case Quality::Minor: return {0, 3, 7};
        case Quality::Diminished: return {0, 3, 6};
    }
    return {0, 4, 7};
}

std::string base_suffix(Quality q) {
    switch (q) {
        case Quality::Major: return "";
        case Quality::Minor: return "m";
        case Quality::Diminished: return "dim";
    }
    return "";
}

struct ColourResult {
    std::vector<int> intervals;
    std::string suffix;
    std::string label;
};

ColourResult normal_colour(Quality q, Direction d) {
    if (d == Direction::Center) return {triad(q), base_suffix(q), "BASE"};
    switch (d) {
        case Direction::Up:
            return q == Quality::Major ? ColourResult{{0, 3, 7}, "m", "FLIP"}
                                       : ColourResult{{0, 4, 7}, "", "FLIP"};
        case Direction::UpRight:
            return q == Quality::Minor ? ColourResult{{0, 3, 7, 10}, "m7", "7TH"}
                                       : ColourResult{{0, 4, 7, 10}, "7", "DOM7"};
        case Direction::Right:
            return q == Quality::Major ? ColourResult{{0, 4, 7, 11}, "maj7", "MAJ7"}
                                       : ColourResult{{0, 3, 7, 10}, "m7", "MIN7"};
        case Direction::DownRight:
            return q == Quality::Minor ? ColourResult{{0, 3, 7, 10, 14}, "m9", "9TH"}
                                       : ColourResult{{0, 4, 7, 14}, "add9", "ADD9"};
        case Direction::Down:
            return {{0, 5, 7}, "sus4", "SUS4"};
        case Direction::DownLeft:
            return q == Quality::Major ? ColourResult{{0, 4, 7, 9}, "6", "6TH"}
                                       : ColourResult{{0, 2, 7}, "sus2", "SUS2"};
        case Direction::Left:
            return q == Quality::Major ? ColourResult{{0, 3, 7}, "m", "DARK"}
                                       : ColourResult{{0, 3, 6}, "dim", "DIM"};
        case Direction::UpLeft:
            return {{0, q == Quality::Minor ? 3 : 4, 8}, q == Quality::Minor ? "m+" : "aug", "AUG"};
        case Direction::Center: break;
    }
    return {triad(q), base_suffix(q), "BASE"};
}

ColourResult extended_colour(Quality q, Direction d) {
    if (d == Direction::Center) return {triad(q), base_suffix(q), "BASE"};
    switch (d) {
        case Direction::Up: return {{0, q == Quality::Major ? 4 : 3, 7, 17}, q == Quality::Major ? "add11" : "m(add11)", "ADD11"};
        case Direction::UpRight: return {{0, 4, 7, 10, 14}, "9", "DOM9"};
        case Direction::Right: return {{0, 4, 7, 9, 14}, "6/9", "6/9"};
        case Direction::DownRight: return {{0, 3, 7, 10, 14, 17}, "m11", "MIN11"};
        case Direction::Down: return {{0, 5, 7, 10}, "7sus4", "7SUS"};
        case Direction::DownLeft: return {{0, 3, 6, 10}, "m7b5", "HALF-DIM"};
        case Direction::Left: return {{0, 3, 7, 11}, "mMaj7", "mMAJ7"};
        case Direction::UpLeft: return {{0, 4, 7, 11, 18}, "maj7#11", "LYDIAN"};
        case Direction::Center: break;
    }
    return {triad(q), base_suffix(q), "BASE"};
}

ColourResult dark_colour(Quality q, Direction d) {
    if (d == Direction::Center) return {triad(q), base_suffix(q), "BASE"};
    switch (d) {
        case Direction::Up: return {{0, 7, 12}, "5", "POWER"};
        case Direction::UpRight: return {{0, 4, 7, 10, 15}, "7#9", "CRUNCH"};
        case Direction::Right: return {{0, 5, 10, 15}, "quartal", "QUARTAL"};
        case Direction::DownRight: return {{0, q == Quality::Major ? 4 : 3, 7, 13}, "addb9", "CLUSTER"};
        case Direction::Down: return {{0, 5, 7, 12}, "no3add4", "OPEN"};
        case Direction::DownLeft: return {{0, 3, 7, 9}, "m6", "MIN6"};
        case Direction::Left: return {{0, 6, 12}, "tritone", "TRITONE"};
        case Direction::UpLeft: return {{0, 4, 8, 11}, "+maj7", "AUGMAJ7"};
        case Direction::Center: break;
    }
    return {triad(q), base_suffix(q), "BASE"};
}

ColourResult apply_colour(Quality q, int palette, Direction d) {
    if (palette == 1) return extended_colour(q, d);
    if (palette == 2) return dark_colour(q, d);
    return normal_colour(q, d);
}

int wrap_pc(int value) {
    value %= 12;
    if (value < 0) value += 12;
    return value;
}

} // namespace

std::string note_name(int pitch_class) {
    static constexpr std::array<const char*, 12> names{
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    return names[static_cast<std::size_t>(wrap_pc(pitch_class))];
}

ChordSpec make_chord(int key_pc,
                     int octave,
                     Face face,
                     int chord_bank,
                     int colour_palette,
                     Direction direction) {
    const BaseChord base = base_for(face, chord_bank);
    const ColourResult colour = apply_colour(base.quality, colour_palette, direction);
    ChordSpec chord;
    chord.root_pc = wrap_pc(key_pc + base.root_offset);
    chord.base_octave = 3 + octave;
    chord.intervals = colour.intervals;
    chord.name = note_name(chord.root_pc) + colour.suffix;
    chord.degree = base.degree;
    chord.modifier = colour.label;
    return chord;
}

std::vector<int> voice_lead(const ChordSpec& chord, const std::vector<int>& previous) {
    const int root_midi = 12 * (chord.base_octave + 1) + chord.root_pc;
    std::vector<int> base;
    base.reserve(chord.intervals.size());
    for (const int interval : chord.intervals) base.push_back(root_midi + interval);
    std::sort(base.begin(), base.end());
    if (previous.empty() || base.empty()) return base;

    std::vector<int> best = base;
    double best_score = std::numeric_limits<double>::infinity();
    const int n = static_cast<int>(base.size());
    for (int inversion = 0; inversion < n; ++inversion) {
        std::vector<int> candidate = base;
        for (int i = 0; i < inversion; ++i) candidate[static_cast<std::size_t>(i)] += 12;
        std::sort(candidate.begin(), candidate.end());
        for (int shift : {-12, 0, 12}) {
            std::vector<int> shifted = candidate;
            for (int& note : shifted) note += shift;
            double score = 0.0;
            for (const int note : shifted) {
                int nearest = previous.front();
                int distance = std::abs(note - nearest);
                for (const int old : previous) {
                    const int d = std::abs(note - old);
                    if (d < distance) {
                        distance = d;
                        nearest = old;
                    }
                }
                score += static_cast<double>(distance * distance);
            }
            const double center = std::accumulate(shifted.begin(), shifted.end(), 0.0) / static_cast<double>(shifted.size());
            score += std::abs(center - 60.0) * 0.35;
            if (score < best_score) {
                best_score = score;
                best = shifted;
            }
        }
    }
    return best;
}

std::string face_label(int key_pc, Face face, int chord_bank) {
    const ChordSpec chord = make_chord(key_pc, 0, face, chord_bank, 0, Direction::Center);
    return chord.degree + "  " + chord.name;
}

std::string direction_label(int colour_palette, Direction direction) {
    return apply_colour(Quality::Major, colour_palette, direction).label;
}

std::string mode_name(PlayMode mode) {
    switch (mode) {
        case PlayMode::Pad: return "PAD";
        case PlayMode::Strum: return "STRUM";
        case PlayMode::Arp: return "ARP";
        case PlayMode::Pulse: return "PULSE";
    }
    return "PAD";
}

std::string bank_name(int bank) {
    if (bank == 1) return "DIATONIC+";
    if (bank == 2) return "BORROWED";
    return "CORE";
}

std::string palette_name(int palette) {
    if (palette == 1) return "EXTENDED";
    if (palette == 2) return "DARK";
    return "CLASSIC";
}

} // namespace brkchrd
