# BRKCHRD 0.5.1 Sound Design — English

## Design goal

BRKCHRD prioritises chords that remain clear on the Brick speaker, headphones and external output. The early patches used too much bright FM, noise, detune and low-register density. The current factory set reduces those ingredients, compensates output per patch and gives each instrument its own chord-spacing profile.

## Sixteen factory presets

| Preset | Engine | Voicing profile | Intended use |
| --- | --- | --- | --- |
| Amber Keys | Electric Tines | Keys | warm general-purpose chords |
| Analog Bloom | Dual Analog | Pad | wide soft pads |
| Moon Organ | Additive Organ | Organ | mellow sustained harmony |
| Cinema Strings | String Ensemble | Pad | smooth cinematic chords |
| Velvet Choir | Formant Choir | Choir | dark vocal pad |
| Muted Pluck | Resonant Pluck | Pluck | clear short patterns |
| Glass Harp | Phase Mod | Pluck | gentle upper-register glass |
| Noir Reed | Pulse Reed | Keys | smoky restrained chords |
| Dust Piano | Electric Tines | Keys | worn electric piano |
| String Haze | String Ensemble | Pad | slower, wider strings |
| Chapel Air | Additive Organ | Organ | airy organ with space |
| Tape Choir | Formant Choir | Choir | moving worn vocal texture |
| Doom Chords | Power Stack | Heavy | controlled saturated shells |
| Sub Altar | Sub Harmonics | Bass | root/fifth/octave foundation |
| Frozen Glass | Phase Mod | Pluck | brighter unstable glass |
| Wire Pluck | Resonant Pluck | Pluck | dry metallic articulation |

## Why instruments need different voicings

A five-note minor 11 can sound attractive on a wide pad and unusable on a sub oscillator. BRKCHRD separates the chord symbol from its playable orchestration.

- **Keys:** normally four notes; redundant fifths are removed before important sevenths or ninths.
- **Organ:** compact shell voicings avoid additive low-mid buildup.
- **Pad:** up to five notes, with an opened inner voice for width.
- **Choir:** related to Pad but tuned for slower formant movement.
- **Pluck:** fewer notes and a higher register preserve transient clarity.
- **Heavy:** root/fifth/octave plus a useful third or seventh where safe; dense low extensions are discarded.
- **Bass:** root, fifth and octave only, regardless of the displayed extension.

The screen shows the harmonic chord name. The profile changes orchestration, not harmonic intent.

## Chord correctness

The colour engine starts from the actual quality of each degree.

- a minor degree with the ninth colour becomes `m9`, not a major ninth on the same root;
- a diminished degree with a seventh becomes diminished or half-diminished as appropriate;
- 6/9, add9 and seventh colours preserve major, minor or diminished quality;
- interval sets are sorted, unique, rooted and kept inside the supported range.

Automated tests enumerate every face button, bank, palette and D-pad direction.

## Stable and live register modes

### Voice Lead Off

`voice_chord()` generates a deterministic voicing from chord, octave and preset profile. The previous chord is ignored. Raising the octave increases every note by exactly twelve semitones.

### Voice Lead On

BRKCHRD evaluates nearby inversions and octave candidates while rejecting results that drift too far from the register requested by the front controls. Motion cost, range limits and target-centre cost are combined. The result is route-sensitive without freely cancelling an octave command.

## Synthesis models

The current engines are deliberately lightweight and procedural:

- Dual Analog combines softened saw, triangle and pulse/sub body;
- Electric Tines uses a phase-modulated strike and decaying transient;
- Additive Organ combines stable harmonic partials;
- String Ensemble layers detuned ramps and body components;
- Formant Choir moves a simplified vowel/formant mixture;
- Resonant Pluck uses a short decaying excitation;
- Phase Mod creates glass-like spectra with controlled modulation depth;
- Power Stack uses band-limited-feeling oscillator mixtures and soft saturation;
- Pulse Reed combines pulse-width motion and stable fundamentals;
- Sub Harmonics concentrates energy in root-oriented low partials.

Compared with the early sounds, analog patches use less raw saw/square, choir noise is reduced, glass modulation is restrained, Doom saturation is lower, Sub Altar removes upper clutter and detune range is narrower.

## Parameters

- **Tone:** filter cutoff, brightness or high-partial balance;
- **Body:** oscillator mixture, low-mid mass or model density;
- **Motion:** drift, LFO depth and model movement;
- **Attack/Release:** amplitude-envelope times;
- **Spread:** detune and stereo distribution;
- **Master:** preset-loaded output control.

The same control has model-specific behaviour. Presets are safe starting points rather than sample-based instruments.

## Effects

FX1 and FX2 are serial and support Off, Chorus, Phaser, Tremolo, Drive, Crusher, Delay and Reverb. PERF FX temporarily substitutes a two-effect gesture and restores the base chain on release.

`PERF FX = OFF` removes the performance screen from the L1 cycle without disabling the ordinary effect slots.

## Gain safety

Dense chords are normalised before effects and every preset has a level coefficient. Drive, Crusher and extreme PERF FX can still increase perceived loudness. Start Doom Chords and Sub Altar at moderate hardware volume.

## 0.5.1 visual feedback

The sound engine itself is unchanged from 0.5.0. Version 0.5.1 improves the editing feedback: the preset has a large dedicated card, numeric values remain inside the frame, and the selected parameter uses a white bar so its value never disappears against the active-row background.
