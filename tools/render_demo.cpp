#include "brkchrd/music.hpp"
#include "brkchrd/synth.hpp"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    const std::string path = argc > 1 ? argv[1] : "brkchrd-v051-demo.wav";
    constexpr int rate = 48000;
    brkchrd::SynthEngine synth(rate);
    std::vector<float> output;
    std::vector<int> previous;
    const std::vector<brkchrd::Face> progression{brkchrd::Face::A, brkchrd::Face::B, brkchrd::Face::X, brkchrd::Face::Y};
    for (int preset = 0; preset < brkchrd::SynthEngine::preset_count(); ++preset) {
        synth.set_preset(preset);
        const auto chord = brkchrd::make_chord(0, 0, progression[static_cast<std::size_t>(preset % 4)], 0,
                                                preset % 5 == 0 ? 1 : 0,
                                                preset % 5 == 0 ? brkchrd::Direction::UpRight : brkchrd::Direction::Right);
        previous = brkchrd::voice_chord(chord, previous, false, synth.voicing_profile());
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
