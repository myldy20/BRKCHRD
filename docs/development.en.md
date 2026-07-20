# Developing BRKCHRD — English

## Repository map

```text
include/brkchrd/       public C++ interfaces
src/music.cpp          chord construction and voicing
src/synth.cpp          voices, synthesis models and effects
src/sdl_main.cpp       SDL input, UI, configuration and audio callback
tests/test_main.cpp    harmonic, voicing and audio tests
tools/render_demo.cpp  offline preset renderer
tools/package_*.sh     PortMaster and NextUI packagers
packaging/             launchers, metadata and platform artwork
assets/branding/       official project identity
```

## Build dependencies

- CMake 3.16 or later;
- a C++20 compiler;
- SDL2 development files;
- `zip` and `file` for packaging/inspection;
- AArch64 environment or container for handheld releases.

Ubuntu example:

```bash
sudo apt-get install build-essential cmake pkg-config libsdl2-dev zip file
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

## Desktop run

```bash
./build/brkchrd-sdl
```

For headless CI:

```bash
BRKCHRD_SKIP_SPLASH=1 \
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
timeout 3s ./build/brkchrd-sdl
```

## Harmonic invariants

Changes to `music.cpp` must preserve:

- every interval set is sorted and unique;
- interval zero is the harmonic root;
- minor and diminished scale degrees retain their quality under colour changes;
- deterministic voicing is independent of previous chords;
- changing octave in deterministic mode moves every voice exactly 12 semitones;
- live voice-leading candidates remain anchored near the selected octave;
- Bass and Heavy profiles do not reintroduce dense low extensions.

Add a regression test whenever a physical-device report exposes a repeatable sequence.

## Audio rules

The handheld target has limited CPU and a small speaker. Prefer:

- bounded voice counts;
- block-level or shared modulation where perceptually acceptable;
- stable filters and explicit parameter clamping;
- level compensation per model;
- no allocations, file I/O or locks introduced inside per-sample processing;
- soft limiting after the serial effect chain.

Test headphones, line output and the built-in speaker. A sound that is attractive on studio monitors can become harsh or disappear on the Brick.

## UI rules

Logical resolution is 512×384. Text uses the built-in bitmap font.

- values must remain inside their frames;
- right-side values use `text_right` and a safe padding;
- selected parameters must retain a visible value indicator;
- long names must be tested against the longest factory string;
- essential stage information must not rely on subtle colour differences;
- controller labels refer to physical Brick buttons.

## Packaging

PortMaster:

```bash
sh tools/package_portmaster.sh build-aarch64 dist
```

NextUI:

```bash
sh tools/package_nextui.sh build-aarch64 dist
```

Both packages include the same executable, documentation, GPL text, origin notice and third-party notices. Platform launchers only adapt paths and persistent storage.

## Versioning and releases

A release should update:

- `kVersion` in `src/sdl_main.cpp`;
- CMake project version;
- default demo/output archive names;
- README and documentation headings;
- `pak.json` and platform metadata;
- CI artifact names and package inspections;
- release notes.

Do not publish a tag until Linux tests, SDL smoke test, AArch64 tests and both package inspections pass.

## Contributions

Keep changes focused and describe physical-device validation. Do not commit generated build directories or private keys. New contributions are accepted under GPL-3.0-or-later unless agreed otherwise in advance, and redistributed derivatives must preserve the notice in `NOTICE.md`.
