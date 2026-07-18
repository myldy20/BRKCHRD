#include "brkchrd/music.hpp"
#include "brkchrd/synth.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace brkchrd;

namespace {
int failures = 0;
void expect(bool condition, const char* message) {
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}
}

int main() {
    const auto tonic = make_chord(0, 0, Face::A, 0, 0, Direction::Center);
    expect(tonic.name == "C", "A in core bank is C major");
    expect(tonic.intervals == std::vector<int>({0, 4, 7}), "C major triad intervals");
    expect(make_chord(0, 0, Face::B, 0, 0, Direction::Center).name == "G", "B is V / G");
    expect(make_chord(0, 0, Face::X, 0, 0, Direction::Center).name == "Am", "X is vi / Am");
    expect(make_chord(0, 0, Face::Y, 0, 0, Direction::Center).name == "F", "Y is IV / F");
    expect(make_chord(0, 0, Face::A, 1, 0, Direction::Center).name == "Dm", "L1+A is ii / Dm");
    expect(make_chord(0, 0, Face::A, 2, 0, Direction::Center).name == "Cm", "R1+A is borrowed tonic minor");
    expect(make_chord(0, 0, Face::A, 0, 0, Direction::Right).name == "Cmaj7", "D-pad right adds major seven");
    expect(make_chord(0, 0, Face::A, 0, 1, Direction::UpLeft).name == "Cmaj7#11", "extended up-left is lydian");
    expect(make_chord(0, 0, Face::A, 0, 2, Direction::UpRight).name == "C7#9", "dark up-right is crunch");

    const auto first = voice_lead(tonic, {});
    const auto next = voice_lead(make_chord(0, 0, Face::B, 0, 0, Direction::Center), first);
    expect(!first.empty() && !next.empty(), "voice leading returns notes");
    const int max_jump = std::abs(next.front() - first.front());
    expect(max_jump <= 12, "voice leading avoids absurd bass jumps");

    SynthEngine synth(48000.0);
    synth.set_preset(0);
    synth.play_chord(first);
    auto audio = synth.render_copy(4800);
    const float peak = *std::max_element(audio.begin(), audio.end(), [](float a, float b) {
        return std::abs(a) < std::abs(b);
    });
    expect(std::abs(peak) > 0.001f, "synth produces audible samples");
    expect(std::all_of(audio.begin(), audio.end(), [](float v) { return std::isfinite(v); }), "synth output is finite");

    if (failures == 0) {
        std::cout << "All BRKCHRD tests passed\n";
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
