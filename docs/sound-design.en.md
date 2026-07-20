# BRKCHRD 0.5.0 Sound Design — English

## Design goal

Version 0.5 prioritises chords that remain clear on the Brick speaker, headphones and external output. The previous patches used too much bright FM, noise, detune and low-register density. The new factory set reduces those ingredients, compensates output per patch and gives each instrument its own chord-spacing profile.

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

## Why instruments need different chord voicings

A five-note minor 11 can sound attractive on a wide pad and unusable on a sub oscillator. BRKCHRD therefore separates the chord symbol from its playable voicing.

- **Keys:** normally four notes; redundant fifths are removed before important sevenths/ninths.
- **Organ:** compact shell voicings avoid additive low-mid buildup.
- **Pad:** up to five notes, with an opened inner voice for width.
- **Choir:** similar to Pad but tuned for slower formant movement.
- **Pluck:** fewer notes and a higher register, preserving transient clarity.
- **Heavy:** root/fifth/octave plus a useful third or seventh when safe; dense low extensions are discarded.
- **Bass:** root, fifth and octave only, regardless of the displayed extension.

The screen continues to show the harmonic chord name. The profile changes orchestration, not harmonic intent.

## Chord correctness

The colour engine now starts from the actual quality of each degree.

Examples:

- a minor degree with the 9th colour becomes `m9`, not a major ninth chord on the same root;
- a diminished degree with a seventh becomes diminished or half-diminished as appropriate;
- 6/9, add9 and major/minor seventh colours preserve major/minor/diminished quality;
- interval sets are sorted, unique, start at the root and remain within the supported extension range.

Automated tests enumerate every face button, bank, palette and D-pad direction.

## Stable and live register modes

### VOICE LEAD OFF

`voice_chord()` generates a deterministic voicing from chord, octave and preset profile. The previous chord is ignored. Raising octave increases every note by exactly 12 semitones.

### VOICE LEAD ON

BRKCHRD tests nearby inversions and ±12 candidates, but rejects candidates that drift too far from the register requested by the octave control. Motion cost, range limits and target-centre cost are combined. The result remains route-sensitive but no longer freely cancels an octave command.

## Synth changes

Compared with 0.4:

- analog patches use more triangle/sine body and less raw saw/square;
- electric keys use a shorter, lower-index FM transient;
- choir noise is reduced substantially;
- glass modulation depth is restrained in the musical presets;
- Doom saturation is lower before the user effects;
- Sub Altar removes upper harmonic clutter;
- oscillator detune range is reduced;
- active voices are normalised approximately by the square root of chord size;
- each preset has an additional level trim before the master limiter.

## SOUND controls

Normal layer:

- Preset
- Tone
- Body
- Motion

L2-held layer:

- Attack
- Release
- Spread
- BPM
- Play Mode

`Tone`, `Body` and `Motion` still have engine-specific meaning. Presets are intended as safe starting points, not locked sample-based instruments.

## Effects

The two base slots remain Off, Chorus, Phaser, Tremolo, Drive, Crusher, Delay or Reverb. PERF FX temporarily replaces the slots and restores them on release.

`PERF FX = OFF` removes the performance screen from the L1 cycle but does not disable the two base effect slots.

## Gain safety

Dense chords are normalised before FX, and every patch has a level coefficient. Destructive Drive/Crusher/PERF FX combinations can still increase perceived loudness. Test Doom Chords and Sub Altar at moderate hardware volume first.
