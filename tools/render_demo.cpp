#include "brkchrd/music.hpp"
#include "brkchrd/synth.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace brkchrd;

int main(int argc, char** argv) {
    const std::string path = argc > 1 ? argv[1] : "brkchrd-demo.wav";
    constexpr int sample_rate = 48000;
    SynthEngine synth(sample_rate);
    synth.set_preset(0);
    synth.set_mode(PlayMode::Strum);
    synth.set_bpm(92);

    std::vector<float> output;
    std::vector<int> previous;
    const Face progression[] = {Face::A, Face::B, Face::X, Face::Y, Face::A, Face::Y, Face::B, Face::A};
    const Direction colours[] = {Direction::Center, Direction::UpRight, Direction::Right, Direction::DownRight,
                                 Direction::Down, Direction::UpLeft, Direction::DownLeft, Direction::Center};
    for (std::size_t i = 0; i < 8; ++i) {
        const ChordSpec chord = make_chord(0, 0, progression[i], 0, static_cast<int>(i / 3) % 3, colours[i]);
        previous = voice_lead(chord, previous);
        synth.play_chord(previous);
        auto block = synth.render_copy(sample_rate * 2);
        output.insert(output.end(), block.begin(), block.end());
    }
    synth.release_chord();
    auto tail = synth.render_copy(sample_rate * 3);
    output.insert(output.end(), tail.begin(), tail.end());
    if (!write_wav(path, output, sample_rate)) {
        std::cerr << "Could not write " << path << '\n';
        return EXIT_FAILURE;
    }
    std::cout << "Wrote " << path << '\n';
    return EXIT_SUCCESS;
}
