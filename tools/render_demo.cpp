#include "brkchrd/music.hpp"
#include "brkchrd/synth.hpp"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    const std::string path = argc > 1 ? argv[1] : "brkchrd-v020-demo.wav";
    constexpr int rate = 48000;
    brkchrd::SynthEngine synth(rate);
    std::vector<float> output;
    std::vector<int> previous;
    const std::vector<brkchrd::Face> progression{brkchrd::Face::A, brkchrd::Face::Y, brkchrd::Face::X, brkchrd::Face::B};
    for (int preset : {0,1,2,3,4,5,6,7,8,9}) {
        synth.set_preset(preset);
        const auto chord = brkchrd::make_chord(0, 0, progression[static_cast<std::size_t>(preset % 4)], 0, preset == 7 ? 2 : 0, preset == 7 ? brkchrd::Direction::Up : brkchrd::Direction::DownRight);
        previous = brkchrd::voice_lead(chord, previous);
        synth.play_chord(previous);
        auto section = synth.render_copy(static_cast<std::size_t>(rate * 2));
        output.insert(output.end(), section.begin(), section.end());
        synth.release_chord();
        auto tail = synth.render_copy(static_cast<std::size_t>(rate / 2));
        output.insert(output.end(), tail.begin(), tail.end());
    }
    if (!brkchrd::write_wav(path, output, rate)) { std::cerr << "Could not write " << path << '\n'; return 1; }
    std::cout << "Wrote " << path << '\n';
    return 0;
}
