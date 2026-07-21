#include "brkchrd/music.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numeric>

namespace brkchrd {
namespace {

enum class Quality { Major, Minor, Diminished };
struct BaseChord { int root_offset = 0; Quality quality = Quality::Major; const char* degree = "I"; };

BaseChord base_for(Face face, int bank) {
    const int f = static_cast<int>(face);
    if (bank == 1) {
        constexpr std::array<BaseChord, 4> values{{
            {2, Quality::Minor, "ii"}, {4, Quality::Minor, "iii"},
            {11, Quality::Diminished, "viiO"}, {10, Quality::Major, "bVII"},
        }};
        return values[static_cast<std::size_t>(f)];
    }
    if (bank == 2) {
        constexpr std::array<BaseChord, 4> values{{
            {0, Quality::Minor, "i"}, {5, Quality::Minor, "iv"},
            {8, Quality::Major, "bVI"}, {3, Quality::Major, "bIII"},
        }};
        return values[static_cast<std::size_t>(f)];
    }
    constexpr std::array<BaseChord, 4> values{{
        {0, Quality::Major, "I"}, {7, Quality::Major, "V"},
        {9, Quality::Minor, "vi"}, {5, Quality::Major, "IV"},
    }};
    return values[static_cast<std::size_t>(f)];
}

std::vector<int> triad(Quality quality) {
    switch (quality) {
    case Quality::Major: return {0, 4, 7};
    case Quality::Minor: return {0, 3, 7};
    case Quality::Diminished: return {0, 3, 6};
    }
    return {0, 4, 7};
}

std::string base_suffix(Quality quality) {
    switch (quality) {
    case Quality::Major: return "";
    case Quality::Minor: return "m";
    case Quality::Diminished: return "dim";
    }
    return "";
}

struct ColourResult { std::vector<int> intervals; std::string suffix; std::string label; };

ColourResult normal_colour(Quality q, Direction d) {
    if (d == Direction::Center) return {triad(q), base_suffix(q), "BASE"};
    switch (d) {
    case Direction::Up:
        return q == Quality::Major ? ColourResult{{0,3,7},"m","FLIP"} : ColourResult{{0,4,7},"","FLIP"};
    case Direction::UpRight:
        if (q == Quality::Major) return {{0,4,7,10},"7","DOM7"};
        if (q == Quality::Minor) return {{0,3,7,10},"m7","MIN7"};
        return {{0,3,6,9},"dim7","DIM7"};
    case Direction::Right:
        if (q == Quality::Major) return {{0,4,7,11},"maj7","MAJ7"};
        if (q == Quality::Minor) return {{0,3,7,10},"m7","MIN7"};
        return {{0,3,6,10},"m7b5","HALFDIM"};
    case Direction::DownRight:
        if (q == Quality::Major) return {{0,4,7,14},"add9","ADD9"};
        if (q == Quality::Minor) return {{0,3,7,14},"m(add9)","MINADD9"};
        return {{0,3,6,14},"dim(add9)","DIMADD9"};
    case Direction::Down:
        return {{0,5,7},"sus4","SUS4"};
    case Direction::DownLeft:
        if (q == Quality::Major) return {{0,4,7,9},"6","SIXTH"};
        if (q == Quality::Minor) return {{0,3,7,9},"m6","MIN6"};
        return {{0,3,6,9},"dim7","DIM7"};
    case Direction::Left:
        return q == Quality::Major ? ColourResult{{0,3,7},"m","DARK"} : ColourResult{{0,3,6},"dim","DIM"};
    case Direction::UpLeft:
        return {{0,q == Quality::Minor ? 3 : 4,8},q == Quality::Minor ? "m+" : "aug","AUG"};
    case Direction::Center: break;
    }
    return {triad(q), base_suffix(q), "BASE"};
}

ColourResult extended_colour(Quality q, Direction d) {
    if (d == Direction::Center) return {triad(q), base_suffix(q), "BASE"};
    const int third = q == Quality::Major ? 4 : 3;
    const int fifth = q == Quality::Diminished ? 6 : 7;
    switch (d) {
    case Direction::Up:
        return {{0,third,fifth,14},q == Quality::Major ? "add9" : q == Quality::Minor ? "m(add9)" : "dim(add9)","ADD9"};
    case Direction::UpRight:
        if (q == Quality::Major) return {{0,4,7,10,14},"9","NINTH"};
        if (q == Quality::Minor) return {{0,3,7,10,14},"m9","MIN9"};
        return {{0,3,6,10,14},"m9b5","M9B5"};
    case Direction::Right:
        if (q == Quality::Major) return {{0,4,7,9,14},"6/9","SIXNINE"};
        if (q == Quality::Minor) return {{0,3,7,9,14},"m6/9","MINSIXNINE"};
        return {{0,3,6,9,14},"dim7(add9)","DIM69"};
    case Direction::DownRight:
        if (q == Quality::Major) return {{0,4,7,11,14},"maj9","MAJ9"};
        if (q == Quality::Minor) return {{0,3,7,10,14,17},"m11","MIN11"};
        return {{0,3,6,10,14,17},"m11b5","M11B5"};
    case Direction::Down:
        return {{0,5,7,10},"7sus4","7SUS"};
    case Direction::DownLeft:
        return {{0,3,6,10},"m7b5","HALFDIM"};
    case Direction::Left:
        return {{0,3,7,11},"mMaj7","MMAJ7"};
    case Direction::UpLeft:
        return q == Quality::Major ? ColourResult{{0,4,7,11,18},"maj7#11","LYDIAN"}
                                   : ColourResult{{0,3,7,10,17},"m11","ELEVENTH"};
    case Direction::Center: break;
    }
    return {triad(q), base_suffix(q), "BASE"};
}

ColourResult dark_colour(Quality q, Direction d) {
    if (d == Direction::Center) return {triad(q), base_suffix(q), "BASE"};
    switch (d) {
    case Direction::Up: return {{0,7,12},"5","POWER"};
    case Direction::UpRight:
        return q == Quality::Minor ? ColourResult{{0,3,7,10,15},"m7#9","CRUNCH"}
                                   : ColourResult{{0,4,7,10,15},"7#9","CRUNCH"};
    case Direction::Right: return {{0,5,10,15},"quartal","QUARTAL"};
    case Direction::DownRight: return {{0,q == Quality::Major ? 4 : 3,q == Quality::Diminished ? 6 : 7,13},"addb9","TENSION"};
    case Direction::Down: return {{0,5,7,12},"no3add4","OPEN"};
    case Direction::DownLeft: return q == Quality::Major ? ColourResult{{0,4,7,9},"6","SIXTH"}
                                                        : ColourResult{{0,3,7,9},"m6","MIN6"};
    case Direction::Left: return {{0,6,12},"tritone","TRITONE"};
    case Direction::UpLeft: return {{0,4,8,11},"+maj7","AUGMAJ7"};
    case Direction::Center: break;
    }
    return {triad(q), base_suffix(q), "BASE"};
}

ColourResult apply_colour(Quality q, int palette, Direction d) {
    if (palette == 1) return extended_colour(q, d);
    if (palette == 2) return dark_colour(q, d);
    return normal_colour(q, d);
}

int wrap_pc(int value) { value %= 12; return value < 0 ? value + 12 : value; }

double centre_of(const std::vector<int>& notes) {
    if (notes.empty()) return 60.0;
    return std::accumulate(notes.begin(), notes.end(), 0.0) / static_cast<double>(notes.size());
}

void drop_interval(std::vector<int>& intervals, int pitch_class) {
    const auto it = std::find_if(intervals.begin(), intervals.end(), [pitch_class](int interval) {
        return interval != 0 && wrap_pc(interval) == pitch_class;
    });
    if (it != intervals.end()) intervals.erase(it);
}

std::vector<int> prepare_intervals(const ChordSpec& chord, VoicingProfile profile) {
    std::vector<int> intervals = chord.intervals;
    std::sort(intervals.begin(), intervals.end());
    intervals.erase(std::unique(intervals.begin(), intervals.end()), intervals.end());

    if (profile == VoicingProfile::Bass) {
        std::vector<int> bass{0};
        const auto fifth = std::find_if(intervals.begin(), intervals.end(), [](int i) {
            const int pitch_class = wrap_pc(i);
            return pitch_class == 7 || pitch_class == 6;
        });
        if (fifth != intervals.end()) bass.push_back(wrap_pc(*fifth));
        bass.push_back(12);
        return bass;
    }
    if (profile == VoicingProfile::Heavy) {
        std::vector<int> heavy{0};
        const auto third = std::find_if(intervals.begin(), intervals.end(), [](int i) { const int pc = wrap_pc(i); return pc == 3 || pc == 4; });
        const auto fifth = std::find_if(intervals.begin(), intervals.end(), [](int i) { return wrap_pc(i) == 7; });
        const auto seventh = std::find_if(intervals.begin(), intervals.end(), [](int i) { const int pc = wrap_pc(i); return pc == 10 || pc == 11; });
        if (fifth != intervals.end()) heavy.push_back(7);
        if (third != intervals.end() && intervals.size() <= 4U) heavy.push_back(wrap_pc(*third));
        if (seventh != intervals.end()) heavy.push_back(wrap_pc(*seventh) + 12);
        heavy.push_back(12);
        std::sort(heavy.begin(), heavy.end());
        heavy.erase(std::unique(heavy.begin(), heavy.end()), heavy.end());
        return heavy;
    }

    const std::size_t max_notes = profile == VoicingProfile::Pad || profile == VoicingProfile::Choir ? 5U : 4U;
    if (intervals.size() > max_notes) {
        drop_interval(intervals, 7);
    }
    while (intervals.size() > max_notes) {
        if (intervals.size() > 2U) intervals.erase(intervals.begin() + 1);
        else intervals.pop_back();
    }

    if ((profile == VoicingProfile::Pad || profile == VoicingProfile::Choir) && intervals.size() >= 4U) {
        intervals[1] += 12;
        std::sort(intervals.begin(), intervals.end());
    }
    return intervals;
}

std::vector<int> stable_voicing(const ChordSpec& chord, VoicingProfile profile) {
    const int register_offset = profile == VoicingProfile::Bass ? -12
                              : profile == VoicingProfile::Heavy ? -5
                              : profile == VoicingProfile::Pluck ? 7
                              : 0;
    const int root_midi = 12 * (chord.base_octave + 1) + chord.root_pc + register_offset;
    const auto intervals = prepare_intervals(chord, profile);
    std::vector<int> notes;
    notes.reserve(intervals.size());
    for (int interval : intervals) notes.push_back(root_midi + interval);
    std::sort(notes.begin(), notes.end());
    return notes;
}

double range_penalty(const std::vector<int>& notes, VoicingProfile profile) {
    int low = 43;
    int high = 84;
    if (profile == VoicingProfile::Bass) { low = 28; high = 60; }
    else if (profile == VoicingProfile::Heavy) { low = 36; high = 72; }
    else if (profile == VoicingProfile::Pluck) { low = 50; high = 91; }
    double penalty = 0.0;
    for (int note : notes) {
        if (note < low) penalty += static_cast<double>((low - note) * (low - note)) * 5.0;
        if (note > high) penalty += static_cast<double>((note - high) * (note - high)) * 5.0;
    }
    return penalty;
}

} // namespace

std::string note_name(int pitch_class) {
    static constexpr std::array<const char*, 12> names{"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    return names[static_cast<std::size_t>(wrap_pc(pitch_class))];
}

ChordSpec make_chord(int key_pc, int octave, Face face, int chord_bank, int colour_palette, Direction direction) {
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

std::vector<int> voice_chord(const ChordSpec& chord,
                             const std::vector<int>& previous,
                             bool voice_leading,
                             VoicingProfile profile) {
    const std::vector<int> stable = stable_voicing(chord, profile);
    if (!voice_leading || previous.empty() || stable.empty()) return stable;

    const double target_center = centre_of(stable);
    std::vector<int> best = stable;
    double best_score = std::numeric_limits<double>::infinity();
    const int count = static_cast<int>(stable.size());
    for (int inversion = 0; inversion < count; ++inversion) {
        std::vector<int> candidate = stable;
        for (int i = 0; i < inversion; ++i) candidate[static_cast<std::size_t>(i)] += 12;
        std::sort(candidate.begin(), candidate.end());
        for (int shift : {-12, 0, 12}) {
            std::vector<int> shifted = candidate;
            for (int& note : shifted) note += shift;
            const double centre_distance = std::abs(centre_of(shifted) - target_center);
            if (centre_distance > 8.5) continue;
            double score = range_penalty(shifted, profile) + centre_distance * centre_distance * 2.8;
            const std::size_t pairs = std::min(shifted.size(), previous.size());
            for (std::size_t i = 0; i < pairs; ++i) {
                const int distance = shifted[i] - previous[i];
                score += static_cast<double>(distance * distance);
            }
            if (shifted.size() != previous.size()) {
                score += static_cast<double>(std::max(shifted.size(), previous.size()) - pairs) * 18.0;
            }
            if (score < best_score) { best_score = score; best = shifted; }
        }
    }
    return best;
}

std::vector<int> voice_lead(const ChordSpec& chord, const std::vector<int>& previous) {
    return voice_chord(chord, previous, true, VoicingProfile::Keys);
}

std::string face_label(int key_pc, Face face, int chord_bank) {
    const ChordSpec chord = make_chord(key_pc, 0, face, chord_bank, 0, Direction::Center);
    return chord.degree + " " + chord.name;
}

std::string direction_label(int colour_palette, Direction direction) {
    if (direction == Direction::Center) return "BASE";
    if (colour_palette == 1) {
        switch (direction) {
        case Direction::Up: return "ADD9"; case Direction::UpRight: return "9TH"; case Direction::Right: return "6/9";
        case Direction::DownRight: return "MAJ9/M11"; case Direction::Down: return "7SUS"; case Direction::DownLeft: return "HALFDIM";
        case Direction::Left: return "MMAJ7"; case Direction::UpLeft: return "LYD/11"; case Direction::Center: break;
        }
    }
    if (colour_palette == 2) {
        switch (direction) {
        case Direction::Up: return "POWER"; case Direction::UpRight: return "CRUNCH"; case Direction::Right: return "QUARTAL";
        case Direction::DownRight: return "TENSION"; case Direction::Down: return "OPEN"; case Direction::DownLeft: return "SIXTH";
        case Direction::Left: return "TRITONE"; case Direction::UpLeft: return "AUGMAJ7"; case Direction::Center: break;
        }
    }
    return apply_colour(Quality::Major, colour_palette, direction).label;
}

std::string mode_name(PlayMode mode) {
    switch (mode) { case PlayMode::Pad: return "PAD"; case PlayMode::Strum: return "STRUM"; case PlayMode::Arp: return "ARP"; case PlayMode::Pulse: return "PULSE"; }
    return "PAD";
}
std::string bank_name(int bank) { return bank == 1 ? "DIATONIC+" : bank == 2 ? "BORROWED" : "CORE"; }
std::string palette_name(int palette) { return palette == 1 ? "EXTENDED" : palette == 2 ? "DARK" : "CLASSIC"; }
std::string voicing_profile_name(VoicingProfile profile) {
    switch (profile) {
    case VoicingProfile::Keys: return "KEYS"; case VoicingProfile::Pad: return "PAD"; case VoicingProfile::Organ: return "ORGAN";
    case VoicingProfile::Pluck: return "PLUCK"; case VoicingProfile::Choir: return "CHOIR"; case VoicingProfile::Heavy: return "HEAVY";
    case VoicingProfile::Bass: return "BASS";
    }
    return "KEYS";
}

} // namespace brkchrd
